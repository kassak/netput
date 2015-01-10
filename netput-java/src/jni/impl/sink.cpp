#include "../org_netput_Sink.h"
#include "storage.h"
#include <netput/netput.h>

extern "C" {

void throw_already_disposed()
{
}

JNIEXPORT void JNICALL Java_org_netput_Sink_setDevice
  (JNIEnv * env, jobject obj, jlong dev_id)
{
   netput_sink_desc_t * p = get_native_ptr<netput_sink_desc_t>(env, obj);
   if(!p)
   {
      throw_already_disposed();
      return;
   }
   p->set_device((uint32_t)dev_id, p->data);
}

JNIEXPORT void JNICALL Java_org_netput_Sink_setKey
  (JNIEnv * env, jobject obj, jlong key_id, jlong value)
{
   netput_sink_desc_t * p = get_native_ptr<netput_sink_desc_t>(env, obj);
   if(!p)
   {
      throw_already_disposed();
      return;
   }
   p->set_key((uint32_t)key_id, (uint32_t)value, p->data);
}

JNIEXPORT void JNICALL Java_org_netput_Sink_flushDevice
  (JNIEnv * env, jobject obj)
{
   netput_sink_desc_t * p = get_native_ptr<netput_sink_desc_t>(env, obj);
   if(!p)
   {
      throw_already_disposed();
      return;
   }
   p->flush_device(p->data);
}

JNIEXPORT void JNICALL Java_org_netput_Sink_finalize
  (JNIEnv * env, jobject obj)
{
   delete_native_ptr<netput_sink_desc_t>(env, obj);
}

}