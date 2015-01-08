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

struct processor_t
{
   processor_t(size_t gamepad_id)
      : gamepad_id_(gamepad_id)
      , log_(np::log::keywords::channel = "netput.sinks.vjoy")
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
   }
   void flush_device()
   {
      BOOST_LOG_SEV(log_, np::log::trace) << "Flush device";
   }
   void set_key(uint32_t key, uint32_t value)
   {
      BOOST_LOG_SEV(log_, np::log::trace) << "Set key [key=" << key << ", value=" << value << "]";
   }
   /*void process_packet(const np::packet_header_t * pack)
   {
      if(!np::is_packet_valid(pack))
      {
         std::cout << "invalid packet" << std::endl;
         return;
      }
      const np::key_state_header_t * key = NULL;
      while(key = np::next_key_state(pack, key))
         process_key(key);
      flush_state();
   }

   template<class T>
   T extract(const np::key_state_header_t * key)
   {
      dispatcher_t<T> d;
      np::dispatch_value(key, d);
      return d.val;
   }

   void process_key(const np::key_state_header_t * key)
   {
      set_button_state(key->key_id, extract<bool>(key));
   }
   */
   void flush_state()
   {
      bool res = TRUE == UpdateVJD((UINT)gamepad_id_, &data_);
      if(!res)
         std::cout << "Flush failed" << std::endl;
      else
         std::cout << "Flush OK" << std::endl;
      assert(res);
   }

   LONG & get_buttons_word(size_t word_id)
   {
      switch(word_id)
      {
      case 0: return data_.lButtons;
      case 1: return data_.lButtonsEx1;
      case 2: return data_.lButtonsEx2;
      case 3: return data_.lButtonsEx3;
      default: throw std::logic_error("Should not be");
      }
   }

   void set_button_state(size_t id, bool state)
   {
      size_t word_id = id / 32;
      size_t bit_id = id % 32;
      LONG & w = get_buttons_word(word_id);
      if(state)
         w |= 1 << bit_id;
      else
         w &= ~(1 << bit_id);
   }
private:
   JOYSTICK_POSITION_V2 data_;
   size_t gamepad_id_;
   np::log::logger_t log_;
};

extern "C" __declspec(dllexport) int create_sink(const dyn_property_t * prop, netput_sink_desc_t * res)
{
   size_t dev_id = 1;
   if(const dyn_property_t * dev_p = dyn_prop_mapping_find(prop, "device_id"))
      if(const char * dev_s = dyn_prop_get_string(dev_p))
         dev_id = std::strtoul(dev_s, NULL, 10);
   processor_t * proc = new processor_t(dev_id);
   *res = np::make_sink_impl_desc(proc);
   return 1;
}

extern "C" __declspec(dllexport) void free_sink(netput_sink_desc_t * res)
{
   delete static_cast<processor_t*>(res->data);
}