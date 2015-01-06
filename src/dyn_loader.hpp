#pragma once

#include <windows.h>

#include <boost/noncopyable.hpp>
#include <boost/system/system_error.hpp>

struct shared_library_exception_t
   : boost::system::system_error
{
   shared_library_exception_t(int ev, const boost::system::error_category & ecat, const char * what)
      : boost::system::system_error(ev, ecat, what)
   {}
};

struct shared_library_t
   : boost::noncopyable
{
   static void throw_exception(const char * what)
   {
      throw shared_library_exception_t(::GetLastError()
         , boost::system::system_category(), what);
   }
   shared_library_t(const char * path)
      : module_(::LoadLibraryA(path))
   {
      if(!module_)
         throw_exception("Failed to load library");
   }
   ~shared_library_t()
   {
      ::FreeLibrary(module_);
   }

   template<class Ptr>
   Ptr get_function(const char * foo) const
   {
      if(Ptr f = static_cast<Ptr>(::GetProcAddress(foo)))
         return f;
      throw_exception("Failed to get function from library");
   }
private:
   HMODULE module_;
};