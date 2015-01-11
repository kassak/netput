cmake_minimum_required(VERSION 2.8.11)
project(netput-tests)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

enable_testing()
find_package(GTest REQUIRED)

add_executable(test_dyn_properties test_dyn_properties.cpp)
target_link_libraries(test_dyn_properties ${GTEST_BOTH_LIBRARIES} netput)
target_include_directories(test_dyn_properties PRIVATE ${GTEST_INCLUDE_DIRS})
add_test(netput-tests test_dyn_properties)
copy_target_to_target_dir(netput test_dyn_properties POST_BUILD)

add_executable(test_dyn_loader test_dyn_loader.cpp)
target_link_libraries(test_dyn_loader ${GTEST_BOTH_LIBRARIES} netput)
target_include_directories(test_dyn_loader PRIVATE ${GTEST_INCLUDE_DIRS})
add_test(netput-tests test_dyn_loader)
copy_target_to_target_dir(netput test_dyn_loader POST_BUILD)
