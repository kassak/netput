#pragma once
#include <stddef.h>

#ifdef _WIN32
   #ifdef netput_EXPORTS
      #define DYN_PROP_API __declspec(dllexport)
   #else
      #define DYN_PROP_API __declspec(dllimport)
   #endif
#else
   #efine DYN_PROP_API
#endif

#ifdef __cplusplus
extern "C" {
#endif
   enum dyn_property_type
   {
      DYN_PROP_EMPTY,
      DYN_PROP_ROOT,
      DYN_PROP_STRING,
      DYN_PROP_DOUBLE,
   };
   typedef struct dyn_property_t dyn_property_t;

   typedef struct dyn_prop_allocator_t
   {
      void* (*realloc)(void*, size_t, void*);
      void* data;
   } dyn_prop_allocator_t;
   
   DYN_PROP_API dyn_property_t * dyn_prop_create(const dyn_prop_allocator_t * alloc);
   DYN_PROP_API void dyn_prop_free(dyn_property_t * tmp);
   
   DYN_PROP_API dyn_property_t * dyn_prop_add_child(dyn_property_t * prop, const char * name);
   DYN_PROP_API dyn_property_t * dyn_prop_find_or_add_child(dyn_property_t * prop, const char * name);

   DYN_PROP_API const dyn_property_t * dyn_prop_find_child(const dyn_property_t * prop, const char * name);

   DYN_PROP_API void dyn_prop_set_double(dyn_property_t * prop, double val);
   DYN_PROP_API void dyn_prop_set_string(dyn_property_t * prop, const char * str);

   DYN_PROP_API double dyn_prop_get_double(const dyn_property_t * prop);
   DYN_PROP_API char const * dyn_prop_get_string(const dyn_property_t * prop);

   DYN_PROP_API const dyn_property_t * dyn_prop_get_first_child(const dyn_property_t * prop);
   DYN_PROP_API const dyn_property_t * dyn_prop_get_next_sibling(const dyn_property_t * prop);

   DYN_PROP_API int dyn_prop_get_type(const dyn_property_t * prop);
   DYN_PROP_API char const * dyn_prop_get_name(const dyn_property_t * prop);
#ifdef __cplusplus
}
#endif