# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\English_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\English_autogen.dir\\ParseCache.txt"
  "English_autogen"
  )
endif()
