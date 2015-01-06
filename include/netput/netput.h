#pragma once

#include <stdint.h>

extern "C"
{
   struct netput_sink_desc_t
   {
      void (*append)(uint32_t key, uint32_t value, void * data);
      void (*send)(uint32_t dev_id, void * data);
      void * data;
   };
}