#include <netput/dyn_loader.h>
#include <dlfcn.h>
#include "tricks.h"

struct dyn_loader_module_t
{
   void* hmod;
};

DYN_LOADER_API dyn_loader_module_t * dyn_loader_load_module(const char * path)
{
   void* hmod = dlopen(path, RTLD_NOW);
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
   dlclose(module->hmod);
   free(module);
}

DYN_LOADER_API void* dyn_loader_proc_address(const dyn_loader_module_t * module, const char * name)
{
   return dlsym(module->hmod, name);
}

