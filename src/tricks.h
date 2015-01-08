#pragma once
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
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
#endif

static FILE * fopen_trick(const char * path, const char * param)
{
#ifdef _MSC_VER
   wchar_t * wpath;
   wchar_t * wparam;
   FILE * res = NULL;
   wparam = utf8_to_wide(param);
   wpath = utf8_to_wide(path);
   if(wpath && wparam)
      res = _wfopen(wpath, wparam);
   if(wpath)
      free(wpath);
   if(wparam)
      free(wparam);
   return res;
#else
   return fopen(path, param);
#endif
}