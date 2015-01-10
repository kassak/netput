cmake_minimum_required(VERSION 2.8.11)
project(netput)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(opt_netput_build_tests "Build unit tests" OFF)
option(opt_netput_pre_build_libyaml "Use prebuild libyaml" ON)

#find_package(Boost REQUIRED COMPONENTS)
#find_package(Protobuf REQUIRED)
if(opt_netput_pre_build_libyaml)
   find_package(LibYaml REQUIRED)
   add_definitions(-DYAML_DECLARE_STATIC)
else(opt_netput_pre_build_libyaml)
   set(LIBYAML_ROOT NOTFOUND CACHE PATH "Root of libyaml containing CMakeLists.txt")
   if(NOT LIBYAML_ROOT)
      message(FATAL_ERROR "No LIBYAML_ROOT")
   endif()
   add_subdirectory("${LIBYAML_ROOT}" libyaml)
   set(LibYaml_LIBRARIES "$<TARGET_FILE:yaml>")
   set(LibYaml_INCLUDE_DIRS "${LIBYAML_ROOT}/include")
endif(opt_netput_pre_build_libyaml)

set(cpps
   src/loader.c
   src/config_parser.c
   src/dyn_properties.c
)
if(WIN32)
   list(APPEND cpps src/dyn_loader_win.c)
else(WIN32)
   list(APPEND cpps src/dyn_loader_unix.c)
endif(WIN32)

set(hpps
   include/netput/netput.h
   include/netput/config_parser.h
   include/netput/loader.h
   include/netput/dyn_properties.h
   include/netput/dyn_loader.h
)

#include_directories(${CMAKE_CURRENT_BINARY_DIR})
#PROTOBUF_GENERATE_CPP(proto_cpps proto_hpps src/key.proto)

add_library(netput SHARED
   ${cpps} ${hpps}
   #${proto_cpps} ${proto_hpps}
)

target_link_libraries(netput
   PRIVATE #${PROTOBUF_LIBRARIES}
           #${Boost_LIBRARIES}
           ${LibYaml_LIBRARIES}
)
target_include_directories(netput
   PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
   PRIVATE #${PROTOBUF_INCLUDE_DIRS}
           #${Boost_INCLUDE_DIRS}
           ${LibYaml_INCLUDE_DIRS}
)

if(opt_netput_build_tests)
   add_subdirectory(tests)
endif(opt_netput_build_tests)
