#include "../org_netput_Netput.h"
#include "storage.h"
#include "exceptions.h"
#include <netput/loader.h>
#include <netput/dyn_properties.h>
#include <netput/config_parser.h>

static void throw_sink_load_failed(JNIEnv * env, const char * msg)
{
   throw_exception_message(env, "org/netput/Netput$SinkLoadFailedException", msg);
}

static void throw_illegal_argument(JNIEnv * env, const char * message)
{
   throw_exception_message(env, "java/lang/IllegalArgumentException", message);
}

static void throw_config_parse_failed(JNIEnv * env, const char * msg)
{
   throw_exception_message(env, "org/netput/Netput$ConfigParseFailedException", msg);
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
      throw_illegal_argument(env, "props are empty");
      return 0;
   }
   netput_sink_desc_t res;
   if(!netput_load_sink(prop, &res))
   {
      throw_sink_load_failed(env, "unable to load sink");
      return 0;
   }
   jobject sink_obj = create_object(env, "org/netput/Sink", "()V");
   if(!sink_obj)
   {
      netput_free_sink(&res);
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
   {
      throw_config_parse_failed(env, "failed to parse config");
      return NULL;
   }
   jobject prop_obj = create_object(env, "org/netput/DynProperty", "()V");
   if(!prop_obj)
   {
      dyn_prop_free(prop);
      return 0;
   }
   new_native_ptr<dyn_property_t*>(env, prop_obj);
   storage_t<dyn_property_t*>::holder_t * h = get_native_holder_ptr<dyn_property_t*>(env, prop_obj);
   h->obj = prop;
   h->deleter = &dp_deleter;
   return prop_obj;
}

char * copy_string(JNIEnv * env, jbyteArray s)
{
   jbyte * bytes = env->GetByteArrayElements(s, NULL);
   jsize len = env->GetArrayLength(s);
   char * res = (char*)malloc(len + 1);
   memcpy(res, bytes, len);
   res[len] = '\0';
   env->ReleaseByteArrayElements(s, bytes, JNI_ABORT);
   return res;
}

JNIEXPORT jobject JNICALL Java_org_netput_Netput_parseConfigFile
  (JNIEnv * env, jclass, jbyteArray path)
{
   char * s = copy_string(env, path);
   dyn_property_t * prop = netput_parse_config_file(s);
   free(s);
   return parse_config_internal(env, prop);
}

JNIEXPORT jobject JNICALL Java_org_netput_Netput_parseConfigString
  (JNIEnv * env, jclass, jbyteArray content)
{
   char * s = copy_string(env, content);
   dyn_property_t * prop = netput_parse_config_string(s);
   free(s);
   return parse_config_internal(env, prop);
}
