#include <netput/dyn_loader.h>
#include <gtest/gtest.h>

#ifdef _WIN32
#include <windows.h>

TEST(test_dyn_loader, check_load)
{
   dyn_loader_module_t * mod = dyn_loader_load_module("kernel32.dll");
   ASSERT_TRUE(!!mod);
   dyn_loader_unload_module(mod);
   mod = dyn_loader_load_module("some_unexisting_library.");
   ASSERT_FALSE(!!mod);
}

TEST(test_dyn_loader, check_function)
{
   dyn_loader_module_t * mod = dyn_loader_load_module("kernel32.dll");
   ASSERT_TRUE(!!mod);

   typedef DWORD (WINAPI *cd_t)(DWORD, LPSTR);

   cd_t cd = (cd_t)dyn_loader_proc_address(mod, "GetCurrentDirectoryA");

   ASSERT_TRUE(!!cd);
   ASSERT_NE(cd(0, 0), 0);

   dyn_loader_unload_module(mod);
}
#endif