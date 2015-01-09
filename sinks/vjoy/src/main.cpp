#define NOMINMAX
#include <windows.h>
#include <assert.h>
#include <public.h>
#include <vjoyinterface.h>
#include <iostream>
#include <limits>
#include <netput/netput.hpp>
#include <netput/dyn_properties.h>
#include <netput/logging.hpp>
#include <boost/utility/string_ref.hpp>
#include <map>
#define _USE_MATH_DEFINES
#include <math.h>

template<class T>
struct mapping_t
{
   typedef
      std::pair<uint32_t, uint32_t>
      key_type;
   typedef
      std::map<key_type, T>
      mapping_type;
   void add_mapping(uint32_t dev_id, uint32_t key_id, T const & res)
   {
      mapping_[key_type(dev_id, key_id)] = res; 
   }

   const T * map(uint32_t dev_id, uint32_t key_id) const
   {
      typename mapping_type::const_iterator it = mapping_.find(key_type(dev_id, key_id));
      if(it == mapping_.end())
         return NULL;
      return &it->second;
   }
private:
   mapping_type mapping_;
};

enum vjoy_key_type
{
   VJK_BTN, VJK_POV, VJK_AXIS, VJK_UNKNOWN
};

struct vjoy_key_t
{
   vjoy_key_t(){}
   vjoy_key_t(vjoy_key_type type, size_t id, bool hor)
      : type(type), id(id), hor(hor)
   {}
   vjoy_key_type type;
   size_t id;
   bool hor;
};

typedef
   mapping_t<vjoy_key_t>
   vjoy_mapping_t;

vjoy_key_type string_to_vjk(boost::string_ref const & s)
{
   if(s == "button")
      return VJK_BTN;
   if(s == "pov")
      return VJK_POV;
   if(s == "axis")
      return VJK_AXIS;
   return VJK_UNKNOWN;
}

vjoy_mapping_t create_mapping(const dyn_property_t * prop)
{
   vjoy_mapping_t res;
   if(!prop)
      return res;
   for(const dyn_property_t * dev = dyn_prop_get_first_child(prop); dev;
      dev = dyn_prop_get_next_sibling(dev))
   {
      int dev_id;
      if(!dyn_prop_get_int_lexical(dyn_prop_mapping_find(dev, "dev_id"), &dev_id))
         throw std::exception();
      const dyn_property_t * keys = dyn_prop_mapping_find(dev, "keys");
      if(!keys)
         continue;
      for(const dyn_property_t * key = dyn_prop_get_first_child(keys); key;
         key = dyn_prop_get_next_sibling(key))
      {
         if(dyn_prop_get_type(key) != DYN_PROP_PAIR)
            throw std::exception();
         const dyn_property_t * f = dyn_prop_get_pair(key, 1);
         const dyn_property_t * s = dyn_prop_get_pair(key, 0);
         int key_id, vjk_id, hor=0;
         if(!dyn_prop_get_int_lexical(f, &key_id))
            throw std::exception();
         if(!dyn_prop_get_int_lexical(dyn_prop_mapping_find(s, "id"), &vjk_id))
            throw std::exception();
         const dyn_property_t * tp = dyn_prop_mapping_find(s, "type");
         vjoy_key_type vjk;
         if(!tp || !dyn_prop_get_string(tp) || VJK_UNKNOWN == (vjk = string_to_vjk(dyn_prop_get_string(tp))))
            throw std::exception();
         if(!dyn_prop_get_int_lexical(dyn_prop_mapping_find(s, "horizontal"), &hor) && vjk == VJK_POV)
            throw std::exception();
         res.add_mapping(dev_id, key_id, vjoy_key_t(vjk, vjk_id, !!hor));
      }
   }
   return res;
}

struct processor_t
{
   processor_t(size_t gamepad_id, vjoy_mapping_t const & mapping)
      : gamepad_id_(gamepad_id)
      , mapping_(mapping)
      , log_(np::log::keywords::channel = "netput.sinks.vjoy")
      , cdev_(0)
   {
      log_.add_attribute("instance_id", np::log::attributes::make_constant(this));
      log_.add_attribute("vjoy_dev_id", np::log::attributes::make_constant(gamepad_id_));
      BOOST_LOG_SEV(log_, np::log::info) << "Initialization start";
      VjdStat status = GetVJDStatus((UINT)gamepad_id_);
      switch (status) 
      {
      case VJD_STAT_OWN:
         BOOST_LOG_SEV(log_, np::log::critical) << "Already owned";
         throw std::exception();
         break;
      case VJD_STAT_FREE:
         BOOST_LOG_SEV(log_, np::log::info) << "Free";
         break; 
      case VJD_STAT_BUSY:
         BOOST_LOG_SEV(log_, np::log::critical) << "Busy";
         throw std::exception();
         break; 
      case VJD_STAT_MISS:
         BOOST_LOG_SEV(log_, np::log::critical) << "Missing";
         throw std::exception();
         break; 
      default:
         BOOST_LOG_SEV(log_, np::log::critical) << "Unknown state";
         throw std::exception();
         break; 
      };
      if(!AcquireVJD((UINT)gamepad_id_))
      {
         BOOST_LOG_SEV(log_, np::log::critical) << "Acquire failed";
         throw std::exception();
      }
      else
         BOOST_LOG_SEV(log_, np::log::info) << "Acquired";


      std::fill(reinterpret_cast<uint8_t*>(&data_), reinterpret_cast<uint8_t*>(&data_ + 1), 0);
      data_.bDevice = (UINT)gamepad_id_;

      BOOST_LOG_SEV(log_, np::log::info) << "Initialization finished";
   }
   ~processor_t()
   {
      BOOST_LOG_SEV(log_, np::log::info) << "Deinitialization start";
      RelinquishVJD((UINT)gamepad_id_);
      BOOST_LOG_SEV(log_, np::log::info) << "Deinitialization finished";
   }

   void set_device(uint32_t dev_id)
   {
      BOOST_LOG_SEV(log_, np::log::trace) << "Set device [id=" << dev_id << "]";
      cdev_ = dev_id;
   }
   void flush_device()
   {
      BOOST_LOG_SEV(log_, np::log::trace) << "Flush device";
      flush_state();
   }
   void set_key(uint32_t key, uint32_t value)
   {
      BOOST_LOG_SEV(log_, np::log::trace) << "Set key [key=" << key << ", value=" << value << "]";
      const vjoy_key_t * mapped = mapping_.map(cdev_, key);
      if(!mapped)
      {
         BOOST_LOG_SEV(log_, np::log::trace) << "No mapping";
         return;
      }
      switch(mapped->type)
      {
      case VJK_AXIS:
         set_axis(mapped->id, value);
         break;
      case VJK_BTN:
         set_btn(mapped->id, value);
         break;
      case VJK_POV:
         set_pov(mapped->id, value, mapped->hor);
         break;
      }
   }

   int32_t centered(uint32_t val)
   {
      return std::numeric_limits<int32_t>::min() + val;
   }

   LONG * get_axis(size_t id)
   {
      LONG JOYSTICK_POSITION_V2::* axis_list[] = {
         &JOYSTICK_POSITION_V2::wThrottle,
         &JOYSTICK_POSITION_V2::wRudder,
         &JOYSTICK_POSITION_V2::wAileron,
         &JOYSTICK_POSITION_V2::wAxisX,
         &JOYSTICK_POSITION_V2::wAxisY,
         &JOYSTICK_POSITION_V2::wAxisZ,
         &JOYSTICK_POSITION_V2::wAxisXRot,
         &JOYSTICK_POSITION_V2::wAxisYRot,
         &JOYSTICK_POSITION_V2::wAxisZRot,
         &JOYSTICK_POSITION_V2::wSlider,
         &JOYSTICK_POSITION_V2::wDial,
         &JOYSTICK_POSITION_V2::wWheel,
         &JOYSTICK_POSITION_V2::wAxisVX,
         &JOYSTICK_POSITION_V2::wAxisVY,
         &JOYSTICK_POSITION_V2::wAxisVZ,
         &JOYSTICK_POSITION_V2::wAxisVBRX,
         &JOYSTICK_POSITION_V2::wAxisVBRY,
         &JOYSTICK_POSITION_V2::wAxisVBRZ
      };
      if(id >= sizeof(axis_list)/sizeof(axis_list[0]))
         return NULL;
      return &(data_.*axis_list[id]);
   }

   DWORD * get_pov(size_t id)
   {
      DWORD JOYSTICK_POSITION_V2::* pov_list[] = {
         &JOYSTICK_POSITION_V2::bHats,
         &JOYSTICK_POSITION_V2::bHatsEx1,
         &JOYSTICK_POSITION_V2::bHatsEx2,
         &JOYSTICK_POSITION_V2::bHatsEx3
      };
      if(id >= sizeof(pov_list)/sizeof(pov_list[0]))
         return NULL;
      return &(data_.*pov_list[id]);
   }

   LONG * get_buttons_word(size_t word_id)
   {
      LONG JOYSTICK_POSITION_V2::* btn_list[] = {
         &JOYSTICK_POSITION_V2::lButtons,
         &JOYSTICK_POSITION_V2::lButtonsEx1,
         &JOYSTICK_POSITION_V2::lButtonsEx2,
         &JOYSTICK_POSITION_V2::lButtonsEx3
      };
      if(word_id >= sizeof(btn_list)/sizeof(btn_list[0]))
         return NULL;
      return &(data_.*btn_list[word_id]);
   }

   DWORD pov_value(size_t id, bool cont)
   {
      int32_t pov[] = {
         centered(povs_[id][0]),
         centered(povs_[id][1])
      };
      if(cont)
      {
         if(pov[0] == 0 && pov[1] == 0)
            return -1;
         double angle = atan2(pov[0], pov[1])/M_PI*180.0;
         if(angle < 0)
            angle += 360;
         return (DWORD)(angle*100 + 0.5) % 36000;
      }
      else
      {
         enum
         {
            DIR_NEUTRAL = -1,
            DIR_NORTH = 0,
            DIR_EAST = 1,
            DIR_SOUTH = 2,
            DIR_WEST = 3,
         };
         if(pov[0] == 0 && pov[1] == 0)
            return DIR_NEUTRAL;
         int ma = std::abs(pov[0]) > std::abs(pov[1]) ? 0 : 1;
         if(ma == 0)
            return pov[0] > 0 ? DIR_EAST : DIR_WEST;
         else
            return pov[1] > 0 ? DIR_NORTH : DIR_SOUTH;
      }
   }

   void set_btn(size_t id, uint32_t value)
   {
      bool state = centered(value) > 0;
      BOOST_LOG_SEV(log_, np::log::trace) << "Set btn [id=" << id << ", state=" << state << "]";
      size_t word_id = id / 32;
      size_t bit_id = id % 32;
      LONG * w = get_buttons_word(word_id);
      if(!w)
      {
         BOOST_LOG_SEV(log_, np::log::warning) << "No such button: " << id;
         return;
      }
      if(state)
         *w |= 1 << bit_id;
      else
         *w &= ~(1 << bit_id);
      BOOST_LOG_SEV(log_, np::log::trace) << "Set btn - ok [id=" << id << ", state=" << state << "]";
   }

   void set_pov(size_t id, uint32_t value, bool hor)
   {
      int32_t state = centered(value);
      BOOST_LOG_SEV(log_, np::log::trace) << "Set pov [id=" << id << ", state=" << state << ", hor=" << hor << "]";
      DWORD* pov = get_pov(id);
      if(!pov)
      {
         BOOST_LOG_SEV(log_, np::log::warning) << "No such pov: " << id;
         return;
      }
      povs_[id][hor ? 0 : 1] = value;
      *pov = pov_value(id, true);
      BOOST_LOG_SEV(log_, np::log::trace) << "Set pov - ok [id=" << id << ", state=" << state
         << ", hor=" << hor << ", pov=" << *pov << "]";
   }

   void set_axis(size_t id, uint32_t value)
   {
      const LONG MAX_AXIS = 0x8000;
      LONG state = (LONG)(value / (double)std::numeric_limits<uint32_t>::max()*MAX_AXIS + 0.5);
      BOOST_LOG_SEV(log_, np::log::trace) << "Set axis [id=" << id << ", state=" << state << "]";
      LONG* axis = get_axis(id);
      if(!axis)
      {
         BOOST_LOG_SEV(log_, np::log::warning) << "No such axis: " << id;
         return;
      }
      *axis = state;
      BOOST_LOG_SEV(log_, np::log::trace) << "Set axis - ok [id=" << id << ", state=" << state << "]";
   }

   void flush_state()
   {
      bool res = TRUE == UpdateVJD((UINT)gamepad_id_, &data_);
      if(!res)
         std::cout << "Flush failed" << std::endl;
      else
         std::cout << "Flush OK" << std::endl;
      assert(res);
   }
private:
   JOYSTICK_POSITION_V2 data_;
   uint32_t povs_[4][2];
   size_t gamepad_id_;
   vjoy_mapping_t mapping_;
   uint32_t cdev_;
   np::log::logger_t log_;
};

extern "C" __declspec(dllexport) int create_sink(const dyn_property_t * prop, netput_sink_desc_t * res)
{
   size_t dev_id = 1;
   if(const dyn_property_t * dev_p = dyn_prop_mapping_find(prop, "device_id"))
      if(const char * dev_s = dyn_prop_get_string(dev_p))
         dev_id = std::strtoul(dev_s, NULL, 10);
   processor_t * proc = new processor_t(dev_id, create_mapping(dyn_prop_mapping_find(prop, "mappings")));
   *res = np::make_sink_impl_desc(proc);
   return 1;
}

extern "C" __declspec(dllexport) void free_sink(netput_sink_desc_t * res)
{
   delete static_cast<processor_t*>(res->data);
}