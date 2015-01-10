cmake_minimum_required(VERSION 2.8.11)
project(netput-java)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(ANDROID) # it is available by default
#   set(JNI_INCLUDE_DIRS)
#   set(JNI_LIBRARIES)
else(ANDROID)
   find_package(JNI REQUIRED)
endif(ANDROID)

set(cpps
   src/jni/impl/sink.cpp
   src/jni/impl/dyn_property.cpp
   src/jni/impl/netput.cpp
)
set(hpps
   src/jni/org_netput_DynProperty.h
   src/jni/org_netput_Netput.h
   src/jni/org_netput_Sink.h
   src/jni/impl/storage.h
)

add_library(netput-jni SHARED
   ${cpps} ${hpps}
)

target_link_libraries(netput-jni
   PRIVATE netput
           ${JNI_LIBRARIES}
)
target_include_directories(netput-jni
   PRIVATE ${JNI_INCLUDE_DIRS}
)

set(TARGET_NAME netput-java)

find_package(Ant REQUIRED)

add_custom_target(${TARGET_NAME} ALL COMMAND ${CMAKE_COMMAND} -E env
  JAVA_HOME=${ANT_JAVA_HOME} #${CMAKE_COMMAND} -E environment
  ${ANT_EXEC_SCRIPT}
  -f "${CMAKE_CURRENT_SOURCE_DIR}/build.xml"
  "-Dcmake-outdir=${CMAKE_CURRENT_BINARY_DIR}"
  "-Dcmake-jar-target=${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.jar"
  "-Dcmake-netput-lib=$<TARGET_FILE:netput>"
  "-Dcmake-netput-jni-lib=$<TARGET_FILE:netput-jni>"
  jar
  DEPENDS netput-jni
 )
