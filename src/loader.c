#include <netput/loader.h>
#include <netput/dyn_loader.h>
#include <stdlib.h>
#include <string.h>

typedef void (*deleter_t)(netput_sink_desc_t *);
typedef int (*factory_t)(const dyn_property_t *, netput_sink_desc_t *);

typedef struct sink_wrapper_data_t
{
   deleter_t deleter;
   dyn_loader_module_t * mod;
   netput_sink_desc_t chain;
} sink_wrapper_data_t;

static void set_key_wrapper(uint32_t key, uint32_t value, void * data)
{
   sink_wrapper_data_t * wdata = (sink_wrapper_data_t *)data;
   wdata->chain.set_key(key, value, wdata->chain.data);
}

static void set_device_wrapper(uint32_t dev_id, void * data)
{
   sink_wrapper_data_t * wdata = (sink_wrapper_data_t *)data;
   wdata->chain.set_device(dev_id, wdata->chain.data);
}

static void flush_device_wrapper(void * data)
{
   sink_wrapper_data_t * wdata = (sink_wrapper_data_t *)data;
   wdata->chain.flush_device(wdata->chain.data);
}

NETPUT_API int netput_load_sink(const char * library, const dyn_property_t * settings
   , netput_sink_desc_t * res)
{
   int fres = 0;
   const dyn_property_t * module = dyn_prop_find_child(settings, "module");
   if(module && dyn_prop_get_type(module) == DYN_PROP_STRING)
   {
      dyn_loader_module_t * lib = dyn_loader_load_module(dyn_prop_get_string(module));
      const char * factory_name = "create_sink";
      const char * deleter_name = "free_sink";
      const dyn_property_t * node;
      factory_t fac;
      deleter_t del;
      if(!lib)
         return 0;
      if(node = dyn_prop_find_child(settings, "factory_name"))
         if(dyn_prop_get_type(node) == DYN_PROP_STRING)
            factory_name = dyn_prop_get_string(node);
      if(node = dyn_prop_find_child(settings, "deleter_name"))
         if(dyn_prop_get_type(node) == DYN_PROP_STRING)
            deleter_name = dyn_prop_get_string(node);
      fac = (factory_t)dyn_loader_proc_address(lib, factory_name);
      del = (deleter_t)dyn_loader_proc_address(lib, deleter_name);
      if(fac && del)
      {
         netput_sink_desc_t chain = {0};
         if(fac(dyn_prop_find_child(settings, "config"), &chain))
         {
            sink_wrapper_data_t * wdata = (sink_wrapper_data_t *)malloc(sizeof(sink_wrapper_data_t));
            if(wdata)
            {
               res->set_key = &set_key_wrapper;
               res->set_device = &set_device_wrapper;
               res->flush_device = &flush_device_wrapper;
               wdata->mod = lib;
               wdata->chain = chain;
               wdata->deleter = del;
               res->data = wdata;
               fres = !0;
            }
         }
      }
      dyn_loader_unload_module(lib);
   }
   return fres;
}

NETPUT_API void netput_free_sink(netput_sink_desc_t * sink)
{
   sink_wrapper_data_t * wdata = (sink_wrapper_data_t *)sink->data;
   wdata->deleter(&wdata->chain);
   dyn_loader_unload_module(wdata->mod);
   free(wdata);
   memset(sink, 0, sizeof(*sink));
}
