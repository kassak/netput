#include "../org_netput_Netput.h"
#include "storage.h"
#include <netput/loader.h>
#include <netput/dyn_properties.h>

static void throw_invalid_argument()
{
}

static void throw_class_not_found()
{
}

static void throw_sink_load_failed()
{
}

static void throw_no_such_method()
{
}

static void throw_construction_fail()
{
}

static void sink_deleter(netput_sink_desc_t & sink)
{
   netput_free_sink(&sink);
}

JNIEXPORT jobject JNICALL Java_org_netput_Netput_loadSink
  (JNIEnv * env, jclass, jobject obj)
{
   dyn_property_t * prop = NULL;
   if(dyn_property_t ** p = get_native_ptr<dyn_property_t*>(env, obj))
      prop = *p;
   if(!prop)
   {
      throw_invalid_argument();
      return 0;
   }
   netput_sink_desc_t res;
   if(!netput_load_sink(prop, &res))
   {
      throw_sink_load_failed();
      return 0;
   }
   jclass sink_cls = env->FindClass("org/netput/Sink");
   if(!sink_cls)
   {
      throw_class_not_found();
      return 0;
   }
   jmethodID sink_constructor = env->GetMethodID(sink_cls, "<init>", "()V");
   if(!sink_constructor)
   {
      throw_no_such_method();
      return 0;
   }
   jobject sink_obj = env->NewObject(sink_cls, sink_constructor);
   if(!sink_obj)
   {
      throw_construction_fail();
      return 0;
   }
   new_native_ptr<netput_sink_desc_t>(env, sink_obj);
   storage_t<netput_sink_desc_t>::holder_t * h = get_native_holder_ptr<netput_sink_desc_t>(env, obj);
   h->obj = res;
   h->deleter = &sink_deleter;
   return sink_obj;
}
