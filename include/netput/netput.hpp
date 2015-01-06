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
         d.append = &sink_impl_t<Impl>::append;
         d.send = &sink_impl_t<Impl>::send;
         d.data = static_cast<void*>(impl);
         return d;
      }
      static void append(uint32_t key, uint32_t value, void * data)
      {
         static_cast<Impl*>(data)->append(key, value);
      }
      static void send(uint32_t dev_id, void * data)
      {
         static_cast<Impl*>(data)->send(dev_id);
      }
   };

   struct sink_wrapper_t
   {
      sink_wrapper_t(){}
      sink_wrapper_t(netput_sink_desc_t const & desc)
         : desc_(desc)
      {
      }

      void append(uint32_t key, uint32_t value)
      {
         desc_.append(key, value, desc_.data);
      }
      void send(uint32_t dev_id)
      {
         desc_.send(dev_id, desc_.data);
      }
   private:
      netput_sink_desc_t desc_;
   };
}
