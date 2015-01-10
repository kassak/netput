if(WIN32)
  find_file(ANT_EXEC_SCRIPT NAMES ant.bat)
else(WIN32)
  find_file(ANT_EXEC_SCRIPT NAMES ant)
endif(WIN32)

include (FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Ant DEFAULT_MSG
  ANT_EXEC_SCRIPT)

