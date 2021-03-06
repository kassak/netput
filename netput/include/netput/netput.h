#pragma once

#include <stdint.h>

#ifdef _WIN32
   #ifdef netput_EXPORTS
      #define NETPUT_API __declspec(dllexport)
   #else
      #define NETPUT_API __declspec(dllimport)
   #endif
#else
   #define NETPUT_API
#endif

#ifdef __cplusplus
extern "C" {
#endif
   typedef struct netput_sink_desc_t
   {
      void (*set_device)(uint32_t dev_id, void * data);
      void (*set_key)(uint32_t key, uint32_t value, void * data);
      void (*flush_device)(void * data);
      void * data;
   } netput_sink_desc_t;
#ifdef __cplusplus
}
#endif
