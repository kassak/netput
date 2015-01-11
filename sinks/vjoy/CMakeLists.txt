cmake_minimum_required(VERSION 2.8.11)
project(netput-sink-vjoy)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(opt_sink_vjoy_build_tests "Build unit tests" OFF)

find_package(vJoy REQUIRED)
find_package(Boost REQUIRED
   COMPONENTS log system filesystem date_time
              thread
)

set(cpps
   src/main.cpp
)

set(hpps
)

add_library(netput-sink-vjoy MODULE
   ${cpps} ${hpps}
)

target_link_libraries(netput-sink-vjoy
   PRIVATE netput
           ${vJoy_LIBRARIES}
           ${Boost_LIBRARIES}
)
target_include_directories(netput-sink-vjoy
   PUBLIC 
   PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
           ${vJoy_INCLUDE_DIRS}
           ${Boost_INCLUDE_DIRS}
)

add_custom_command(TARGET netput-sink-vjoy POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${vJoy_DLL}"
      "$<TARGET_FILE_DIR:netput-sink-vjoy>"
)

if(opt_sink_vjoy_build_tests)
   add_subdirectory(tests)
endif(opt_sink_vjoy_build_tests)

netput_install_default(netput-sink-vjoy)
