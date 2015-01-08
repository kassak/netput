#pragma once
#include "netput.h"

namespace np
{
   template<class Impl>
   struct sink_impl_t
   {
      static netput_sink_desc_t desc(Impl * impl)
      {
         netput_sink_desc_t d;
         d.set_key = &sink_impl_t<Impl>::set_key;
         d.set_device = &sink_impl_t<Impl>::set_device;
         d.flush_device = &sink_impl_t<Impl>::flush_device;
         d.data = static_cast<void*>(impl);
         return d;
      }
      static void set_key(uint32_t key, uint32_t value, void * data)
      {
         static_cast<Impl*>(data)->set_key(key, value);
      }
      static void set_device(uint32_t dev_id, void * data)
      {
         static_cast<Impl*>(data)->set_device(dev_id);
      }
      static void flush_device(void * data)
      {
         static_cast<Impl*>(data)->flush_device();
      }
   };

   struct sink_wrapper_t
   {
      sink_wrapper_t(){}
      sink_wrapper_t(netput_sink_desc_t const & desc)
         : desc_(desc)
      {
      }

      void set_key(uint32_t key, uint32_t value)
      {
         desc_.set_key(key, value, desc_.data);
      }
      void set_device(uint32_t dev_id)
      {
         desc_.set_device(dev_id, desc_.data);
      }
      void flush_device()
      {
         desc_.flush_device(desc_.data);
      }
   private:
      netput_sink_desc_t desc_;
   };
}
