cmake_minimum_required(VERSION 3.1) #due to cmake -E env
#cmake_minimum_required(VERSION 2.8.11)
project(netput-source-android)
set(TARGET_NAME netput-source-android)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Ant REQUIRED)
find_package(AndroidSDK REQUIRED)

if(JAVA_HOME)
  set(_JAVA_HOME "${JAVA_HOME}")
  message("set java_home to ${JAVA_HOME}")
else(JAVA_HOME)
  set(_JAVA_HOME "$ENV{JAVA_HOME}")
endif(JAVA_HOME)

add_custom_target(${TARGET_NAME} ALL COMMAND ${CMAKE_COMMAND} -E env
  JAVA_HOME=${_JAVA_HOME} #${CMAKE_COMMAND} -E environment
  ${ANT_EXEC_SCRIPT}
  -f "${CMAKE_CURRENT_SOURCE_DIR}/ns_android/build.xml"
  "-Dsdk.dir=${ANDROID_SDK_ROOT}"
  "-Dout.dir=${CMAKE_CURRENT_BINARY_DIR}/out"
  "-Dgen.absolute.dir=${CMAKE_CURRENT_BINARY_DIR}/gen"
  debug
 )
