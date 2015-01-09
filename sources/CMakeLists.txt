cmake_minimum_required(VERSION 2.8.11)
project(netput-sources)

option(opt_sources_build_android "Build android source" OFF)
option(opt_sources_build_tester "Tester source" OFF)

if(opt_sources_build_android)
   add_subdirectory(android)
endif(opt_sources_build_android)

if(opt_sources_build_tester)
   add_subdirectory(tester)
endif(opt_sources_build_tester)
