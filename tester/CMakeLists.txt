cmake_minimum_required(VERSION 2.8.11)
project(netput-tester)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Boost REQUIRED
   COMPONENTS program_options
)

set(cpps
   src/main.cpp
)

set(hpps
)

add_executable(netput-tester
   ${cpps} ${hpps}
)

target_link_libraries(netput-tester
   PRIVATE netput
           ${Boost_LIBRARIES}
)
target_include_directories(netput-tester
   PUBLIC 
   PRIVATE ${Boost_INCLUDE_DIRS}
)

if(opt_sink_vjoy_build_tests)
   add_subdirectory(tests)
endif(opt_sink_vjoy_build_tests)
