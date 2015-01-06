#pragma once

#ifdef _WIN32
   #ifdef netput_EXPORTS
      #define DYN_LOADER_API __declspec(dllexport)
   #else
      #define DYN_LOADER_API __declspec(dllimport)
   #endif
#else
   #efine DYN_LOADER_API
#endif

#ifdef __cplusplus
extern "C" {
#endif
   typedef struct dyn_loader_module_t dyn_loader_module_t;

   DYN_LOADER_API dyn_loader_module_t * dyn_loader_load_module(const char * path);
   DYN_LOADER_API void dyn_loader_unload_module(dyn_loader_module_t * module);
   
   DYN_LOADER_API void* dyn_loader_proc_address(const dyn_loader_module_t * module, const char * name);
   DYN_LOADER_API int dyn_loader_last_error();
#ifdef __cplusplus
}
#endif