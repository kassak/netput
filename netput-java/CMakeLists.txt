cmake_minimum_required(VERSION 2.8.11)
project(netput-java)

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

find_package(Gradle REQUIRED)

file(GLOB_RECURSE java_sources src/java/*.java)
list(APPEND java_sources
   build.gradle
)

set(netput-java-jar "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.jar")

add_custom_command(OUTPUT ${netput-java-jar}
   COMMAND ${CMAKE_COMMAND} -E env
      JAVA_HOME=${GRADLE_JAVA_HOME} #${CMAKE_COMMAND} -E environment
      ${GRADLE_EXEC_SCRIPT}
      "--project-cache-dir=${CMAKE_CURRENT_BINARY_DIR}/.gradle"
      -p "${CMAKE_CURRENT_SOURCE_DIR}"
      "-Pcmake.out=${CMAKE_CURRENT_BINARY_DIR}"
      "-Pcmake.jar=${netput-java-jar}"
      jar
   COMMENT "Building jar..."
   DEPENDS ${java_sources}
)

add_custom_target(${TARGET_NAME} ALL DEPENDS ${netput-java-jar}
   SOURCES ${java_sources}
)

set_property(TARGET ${TARGET_NAME} PROPERTY TARGET_FILE ${netput-java-jar})

copy_target_to_dir(netput netput-java ${CMAKE_CURRENT_BINARY_DIR} POST_BUILD)
copy_target_to_dir(netput-jni netput-java ${CMAKE_CURRENT_BINARY_DIR} POST_BUILD)

netput_install_default(netput-jni)
netput_install_files(BIN ${netput-java-jar})

