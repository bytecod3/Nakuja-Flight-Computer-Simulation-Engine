# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\qt-serial-write-test_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\qt-serial-write-test_autogen.dir\\ParseCache.txt"
  "qt-serial-write-test_autogen"
  )
endif()
