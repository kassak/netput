find_file(ANDROID_SDK_ROOT NAMES tools/ant/build.xml)

include (FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(AndroidSDK DEFAULT_MSG
  ANDROID_SDK_ROOT)

