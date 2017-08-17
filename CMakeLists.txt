cmake_minimum_required(VERSION 3.5)

project(gambatte)

include(CTest)

add_subdirectory(libgambatte)

if (BUILD_TESTING)
  add_subdirectory(test)
  enable_testing()
endif ()
