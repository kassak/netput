# Try to find netput
#
# netput_ROOT_DIR - set this to root of netput in case of problems
#
# Once done, this will define
#
#  netput_FOUND - system has netput
#  netput_INCLUDE_DIRS - the netput include directories
#  netput_LIBRARIES - link these to use netput

find_path(netput_ROOT_DIR
  NAMES inc/vjoyinterface.h inc/public.h
)

find_path(netput_INCLUDE_DIRS
  NAMES netput.h
  HINTS ${netput_ROOT_DIR}
)

set(netput_LIBRARIES "")

IF (netput_INCLUDE_DIRS AND netput_LIBRARIES)
  SET(netput_FOUND "YES")
ELSE (netput_INCLUDE_DIRS AND netput_LIBRARIES)
  SET(netput_FOUND "NO")
ENDIF (netput_INCLUDE_DIRS AND netput_LIBRARIES)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(netput DEFAULT_MSG netput_INCLUDE_DIRS) # netput_LIBRARIES