if(WIN32)
  find_file(ANT_EXEC_SCRIPT NAMES ant.bat)
else(WIN32)
  find_file(ANT_EXEC_SCRIPT NAMES ant)
endif(WIN32)

set(ANT_JAVA_HOME "$ENV{JAVA_HOME}" CACHE PATH "JAVA_HOME used for ant execution")

include (FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Ant DEFAULT_MSG
  ANT_EXEC_SCRIPT)

