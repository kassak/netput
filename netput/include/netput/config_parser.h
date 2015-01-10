#pragma once
#include "dyn_properties.h"

#ifdef __cplusplus
extern "C" {
#endif
   DYN_PROP_API dyn_property_t * netput_parse_config_file(const char * path);
   DYN_PROP_API dyn_property_t * netput_parse_config_string(const char * config_str);
#ifdef __cplusplus
}
#endif
