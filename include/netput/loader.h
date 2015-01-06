#pragma once
#include "netput.h"
#include "dyn_properties.h"

#ifdef __cplusplus
extern "C" {
#endif
   NETPUT_API int netput_load_sink(const char * library, const dyn_property_t * settings
      , netput_sink_desc_t * res);
   NETPUT_API void netput_free_sink(netput_sink_desc_t * sink);
#ifdef __cplusplus
}
#endif
