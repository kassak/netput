#pragma once
#include <stddef.h>

#ifdef _WIN32
   #ifdef netput_EXPORTS
      #define DYN_PROP_API __declspec(dllexport)
   #else
      #define DYN_PROP_API __declspec(dllimport)
   #endif
#else
   #define DYN_PROP_API
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
      DYN_PROP_PAIR,
   };
   typedef struct dyn_property_t dyn_property_t;

   typedef struct dyn_prop_allocator_t
   {
      void* (*realloc)(void*, size_t, void*);
      void* data;
   } dyn_prop_allocator_t;

   DYN_PROP_API dyn_property_t * dyn_prop_create(const dyn_prop_allocator_t * alloc);
   DYN_PROP_API dyn_property_t * dyn_prop_clone(const dyn_property_t * prop);
   DYN_PROP_API void dyn_prop_clone_to(const dyn_property_t * prop, dyn_property_t * to);
   DYN_PROP_API void dyn_prop_free(dyn_property_t * tmp);

   DYN_PROP_API const dyn_property_t * dyn_prop_get_next_sibling(const dyn_property_t * prop);

   DYN_PROP_API dyn_property_t * dyn_prop_add_child(dyn_property_t * prop);
   DYN_PROP_API dyn_property_t * dyn_prop_access_pair(dyn_property_t * prop, int first);
   DYN_PROP_API void dyn_prop_set_double(dyn_property_t * prop, double val);
   DYN_PROP_API void dyn_prop_set_string(dyn_property_t * prop, const char * str);
   DYN_PROP_API void dyn_prop_set_stringn(dyn_property_t * prop, const char * str, size_t n);

   DYN_PROP_API const dyn_property_t * dyn_prop_get_first_child(const dyn_property_t * prop);
   DYN_PROP_API const dyn_property_t * dyn_prop_get_pair(const dyn_property_t * prop, int first);
   DYN_PROP_API double dyn_prop_get_double(const dyn_property_t * prop);
   DYN_PROP_API char const * dyn_prop_get_string(const dyn_property_t * prop);

   DYN_PROP_API int dyn_prop_get_type(const dyn_property_t * prop);
   DYN_PROP_API void dyn_prop_set_type(dyn_property_t * prop, int type);

   DYN_PROP_API dyn_property_t * dyn_prop_append_sibling(dyn_property_t * prop);
   DYN_PROP_API const dyn_property_t * dyn_prop_mapping_find(const dyn_property_t * prop, const char * name);
#ifdef __cplusplus
}
#endif