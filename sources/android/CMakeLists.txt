cmake_minimum_required(VERSION 3.1) #due to cmake -E env
#cmake_minimum_required(VERSION 2.8.11)
project(netput-source-android)
set(TARGET_NAME netput-source-android)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Gradle REQUIRED)
find_package(AndroidSDK REQUIRED)

set(dep_libs "${CMAKE_CURRENT_BINARY_DIR}/output-libs/")
set(target_dep_libs "${dep_libs}/${ANDROID_ABI}")

set(target_file "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.apk")

file(GLOB_RECURSE sources *)

set(netput-source-android-apk "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.apk")

add_custom_target("${TARGET_NAME}-files" ALL)
add_custom_target(${TARGET_NAME} ALL DEPENDS ${netput-source-android-apk}
   SOURCES ${sources}
)

file(MAKE_DIRECTORY ${target_dep_libs})
copy_target_to_dir(netput "${TARGET_NAME}-files" ${target_dep_libs} PRE_BUILD)
copy_target_to_dir(netput-jni "${TARGET_NAME}-files" ${target_dep_libs} PRE_BUILD)
copy_target_to_dir2(netput-java "${TARGET_NAME}-files" ${dep_libs} PRE_BUILD)

set_property(TARGET ${TARGET_NAME} PROPERTY TARGET_FILE ${netput-source-android-apk})

add_custom_command(OUTPUT ${netput-source-android-apk}
   COMMAND ${CMAKE_COMMAND} -E env
      JAVA_HOME=${GRADLE_JAVA_HOME}
      ${GRADLE_EXEC_SCRIPT}
      "--project-cache-dir=${CMAKE_CURRENT_BINARY_DIR}/.gradle"
      -p "${CMAKE_CURRENT_SOURCE_DIR}"
      "-Pcmake.out=${CMAKE_CURRENT_BINARY_DIR}"
      "-Pcmake.libs=${dep_libs}"
      "-Pcmake.apk=${netput-source-android-apk}"
      cmakeBuildArmDebug
   COMMENT "Building apk..."
   DEPENDS ${sources} "${TARGET_NAME}-files"
)


netput_install_files(BIN ${netput-source-android-apk})

