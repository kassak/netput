cmake_minimum_required(VERSION 2.8.11)
project(netput-source-tester)
set(TARGET_NAME netput-source-tester)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Boost REQUIRED
   COMPONENTS program_options
)

set(cpps
   src/main.cpp
)

set(hpps
)

add_executable(${TARGET_NAME}
   ${cpps} ${hpps}
)

target_link_libraries(${TARGET_NAME}
   PRIVATE netput
           ${Boost_LIBRARIES}
)
target_include_directories(${TARGET_NAME}
   PUBLIC 
   PRIVATE ${Boost_INCLUDE_DIRS}
)

