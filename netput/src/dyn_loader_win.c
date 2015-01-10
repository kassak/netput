#include <netput/dyn_loader.h>
#include <windows.h>
#include "tricks.h"

struct dyn_loader_module_t
{
   HMODULE hmod;
};

DYN_LOADER_API dyn_loader_module_t * dyn_loader_load_module(const char * path)
{
   HMODULE hmod;
   wchar_t * wpath = utf8_to_wide(path);
   if(!wpath)
      return NULL;
   hmod = LoadLibraryExW(wpath, NULL, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | LOAD_LIBRARY_SEARCH_APPLICATION_DIR);
   free(wpath);
   if(hmod)
   {
      dyn_loader_module_t * mod = (dyn_loader_module_t *)malloc(sizeof(dyn_loader_module_t));
      if(mod)
      {
         mod->hmod = hmod;
      }
      return mod;
   }
   return NULL;
}
DYN_LOADER_API void dyn_loader_unload_module(dyn_loader_module_t * module)
{
   FreeLibrary(module->hmod);
   free(module);
}

DYN_LOADER_API void* dyn_loader_proc_address(const dyn_loader_module_t * module, const char * name)
{
   return GetProcAddress(module->hmod, name);
}

