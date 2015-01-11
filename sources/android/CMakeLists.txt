cmake_minimum_required(VERSION 3.1) #due to cmake -E env
#cmake_minimum_required(VERSION 2.8.11)
project(netput-source-android)
set(TARGET_NAME netput-source-android)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Ant REQUIRED)
find_package(AndroidSDK REQUIRED)

set(dep_libs "${CMAKE_CURRENT_SOURCE_DIR}/libs")

set(target_file "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.apk")

#Need to override
# - sdk.dir
# - out.dir
# - gen.absolute.dir
# - jar.libs.dir
# - native.libs.absolute.dir
# - out.final.file

set(build_overrides
   "-Dsdk.dir=${ANDROID_SDK_ROOT}"
   "-Dout.dir=${CMAKE_CURRENT_BINARY_DIR}/out"
   "-Dgen.absolute.dir=${CMAKE_CURRENT_BINARY_DIR}/gen"
   "-Djar.libs.dir=${dep_libs}"
   "-Dnative.libs.absolute.dir=${dep_libs}"
   
   "-Dout.final.file=${target_file}"
)

add_custom_target(${TARGET_NAME} ALL COMMAND ${CMAKE_COMMAND} -E env
  JAVA_HOME=${ANT_JAVA_HOME} #${CMAKE_COMMAND} -E environment
  ${ANT_EXEC_SCRIPT}
  -f "${CMAKE_CURRENT_SOURCE_DIR}/build.xml"
  ${build_overrides}
  debug
)

set_property(TARGET ${TARGET_NAME} PROPERTY TARGET_FILE ${target_file})

copy_target_to_dir(netput ${TARGET_NAME} ${dep_libs} PRE_BUILD)
copy_target_to_dir(netput-jni ${TARGET_NAME} ${dep_libs} PRE_BUILD)
copy_target_to_dir2(netput-java ${TARGET_NAME} ${dep_libs} PRE_BUILD)

netput_install_files(BIN ${target_file})
