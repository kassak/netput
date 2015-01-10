cmake_minimum_required(VERSION 2.8.11)
project(netput-all)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(opt_build_netput_java "Build netput java library" OFF)

add_subdirectory(netput)
if(opt_build_netput_java)
   add_subdirectory(netput-java)
endif(opt_build_netput_java)
add_subdirectory(sinks)
add_subdirectory(sources)
