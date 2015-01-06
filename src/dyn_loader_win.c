#include <netput/dyn_loader.h>
#include <windows.h>

struct dyn_loader_module_t
{
   HMODULE hmod;
};

static wchar_t * utf8_to_wide(const char * s)
{
   wchar_t * res;

   int sz = MultiByteToWideChar(CP_UTF8, 0, s, -1, NULL, 0);
   if(sz <= 0)
      return NULL;
   res = (wchar_t*)malloc(sz*sizeof(wchar_t));
   sz = MultiByteToWideChar(CP_UTF8, 0, s, -1, res, sz);
   if(sz <= 0)
   {
      free(res);
      return NULL;
   }
   return res;
}

DYN_LOADER_API dyn_loader_module_t * dyn_loader_load_module(const char * path)
{
   HMODULE hmod;
   wchar_t * wpath = utf8_to_wide(path);
   if(!wpath)
      return NULL;
   hmod = LoadLibraryW(wpath);
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
DYN_LOADER_API int dyn_loader_last_error()
{
   return GetLastError();
}

