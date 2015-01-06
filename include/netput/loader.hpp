#pragma once
#include "netput.h"
#include "dyn_properties.h"

extern "C"
{
   netput_sink_desc_t netput_load_sink(const char * library, const dyn_property_t * settings);
}
