#include "../org_netput_DynProperty.h"
#include "storage.h"
#include <netput/dyn_properties.h>

extern "C" {

JNIEXPORT void JNICALL Java_org_netput_DynProperty_finalize
  (JNIEnv * env, jobject obj)
{
   delete_native_ptr<dyn_property_t*>(env, obj);
}

}