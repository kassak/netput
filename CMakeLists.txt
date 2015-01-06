cmake_minimum_required(VERSION 2.8.11)
project(netput)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(opt_build_tests "Build unit tests" OFF)

find_package(Boost REQUIRED COMPONENTS system)
find_package(Protobuf REQUIRED)

set(cpps
   src/loader.c
   src/dyn_properties.c
   src/dyn_loader_win.c
)

set(hpps
   include/netput/netput.h
   include/netput/loader.h
   include/netput/dyn_properties.h
   include/netput/dyn_loader.h
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(proto_cpps proto_hpps src/key.proto)

add_library(netput SHARED
   ${cpps} ${hpps}
   ${proto_cpps} ${proto_hpps}
)

target_link_libraries(netput
   PRIVATE ${PROTOBUF_LIBRARIES}
           ${Boost_LIBRARIES}
)
target_include_directories(netput
   PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
   PRIVATE ${PROTOBUF_INCLUDE_DIRS}
           ${Boost_INCLUDE_DIRS}
)

if(opt_build_tests)
	add_subdirectory(tests)
endif(opt_build_tests)
