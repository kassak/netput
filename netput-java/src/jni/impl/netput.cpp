#include "../org_netput_Netput.h"
#include "storage.h"
#include <netput/loader.h>
#include <netput/dyn_properties.h>
#include <netput/config_parser.h>

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

static void dp_deleter(dyn_property_t * & prop)
{
   dyn_prop_free(prop);
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
      netput_free_sink(&res);
      throw_class_not_found();
      return 0;
   }
   jmethodID sink_constructor = env->GetMethodID(sink_cls, "<init>", "()V");
   if(!sink_constructor)
   {
      netput_free_sink(&res);
      throw_no_such_method();
      return 0;
   }
   jobject sink_obj = env->NewObject(sink_cls, sink_constructor);
   if(!sink_obj)
   {
      netput_free_sink(&res);
      throw_construction_fail();
      return 0;
   }
   new_native_ptr<netput_sink_desc_t>(env, sink_obj);
   storage_t<netput_sink_desc_t>::holder_t * h = get_native_holder_ptr<netput_sink_desc_t>(env, sink_obj);
   h->obj = res;
   h->deleter = &sink_deleter;
   return sink_obj;
}

static jobject parse_config_internal(JNIEnv * env, dyn_property_t * prop)
{
   if(!prop)
      return NULL;
   jclass prop_cls = env->FindClass("org/netput/DynProperty");
   if(!prop_cls)
   {
      dyn_prop_free(prop);
      throw_class_not_found();
      return 0;
   }
   jmethodID prop_constructor = env->GetMethodID(prop_cls, "<init>", "()V");
   if(!prop_constructor)
   {
      dyn_prop_free(prop);
      throw_no_such_method();
      return 0;
   }
   jobject prop_obj = env->NewObject(prop_cls, prop_constructor);
   if(!prop_obj)
   {
      dyn_prop_free(prop);
      throw_construction_fail();
      return 0;
   }
   new_native_ptr<dyn_property_t*>(env, prop_obj);
   storage_t<dyn_property_t*>::holder_t * h = get_native_holder_ptr<dyn_property_t*>(env, prop_obj);
   h->obj = prop;
   h->deleter = &dp_deleter;
   return prop_obj;
}

JNIEXPORT jobject JNICALL Java_org_netput_Netput_parseConfigFile
  (JNIEnv * env, jclass, jbyteArray path)
{
   jbyte * bytes = env->GetByteArrayElements(path, NULL);
   dyn_property_t * prop = netput_parse_config_file(reinterpret_cast<const char *>(bytes));
   env->ReleaseByteArrayElements(path, bytes, JNI_ABORT);
   return parse_config_internal(env, prop);
}

JNIEXPORT jobject JNICALL Java_org_netput_Netput_parseConfigString
  (JNIEnv * env, jclass, jbyteArray content)
{
   jbyte * bytes = env->GetByteArrayElements(content, NULL);
   dyn_property_t * prop = netput_parse_config_file(reinterpret_cast<const char *>(bytes));
   env->ReleaseByteArrayElements(content, bytes, JNI_ABORT);
   return parse_config_internal(env, prop);
}
