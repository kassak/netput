#include <netput/loader.hpp>
#include "dyn_loader.hpp"


extern "C" netput_sink_desc_t netput_load_sink(const char * library, const dyn_property_t * settings)
{
   shared_library_t lib(library);
   netput_sink_desc_t res;
   return res;
}