# Try to find vJoy
#
# vJoy_ROOT_DIR - set this to root of vJoy sdk in case of problems
#
# Once done, this will define
#
#  vJoy_FOUND - system has vJoy
#  vJoy_INCLUDE_DIRS - the vJoy include directories
#  vJoy_LIBRARIES - link these to use vJoy
#  vJoy_DLL - vJoy dll

find_path(vJoy_ROOT_DIR
  NAMES inc/vjoyinterface.h inc/public.h
)

find_path(vJoy_INCLUDE_DIRS
  NAMES vjoyinterface.h public.h
  HINTS ${vJoy_ROOT_DIR}/inc
)

if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
  find_library(vJoy_LIBRARIES
    NAMES vJoyInterface
    HINTS ${vJoy_ROOT_DIR}/lib/amd64
  )
  find_file(vJoy_DLL
    NAMES vJoyInterface.dll
    HINTS ${vJoy_ROOT_DIR}/lib/amd64
  )
else("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
  find_library(vJoy_LIBRARIES
    NAMES vJoyInterface
    HINTS ${vJoy_ROOT_DIR}/lib
  )
  find_file(vJoy_DLL
    NAMES vJoyInterface.dll
    HINTS ${vJoy_ROOT_DIR}/lib
  )
endif("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")

IF (vJoy_INCLUDE_DIRS AND vJoy_LIBRARIES)
  SET(vJoy_FOUND "YES")
ELSE (vJoy_INCLUDE_DIRS AND vJoy_LIBRARIES)
  SET(vJoy_FOUND "NO")
ENDIF (vJoy_INCLUDE_DIRS AND vJoy_LIBRARIES)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(vJoy DEFAULT_MSG vJoy_LIBRARIES vJoy_INCLUDE_DIRS)