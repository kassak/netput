cmake_minimum_required(VERSION 2.8.11)
project(netput-sinks)

option(opt_sinks_build_vjoy "Build vjoy sink(WIN, DInput emulator)" OFF)

if(opt_sinks_build_vjoy)
   add_subdirectory(vjoy)
endif(opt_sinks_build_vjoy)
