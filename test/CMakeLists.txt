cmake_minimum_required(VERSION 3.5)

project(gambatte-test)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(GAMBATTE_QDGBAS "${CMAKE_CURRENT_SOURCE_DIR}/qdgbas.py")

find_package(PNG MODULE REQUIRED)

add_executable(gambatte-testrunner testrunner.cpp)
target_compile_definitions(gambatte-testrunner PUBLIC HAVE_STDINT_H)
target_compile_options(
	gambatte-testrunner
	PRIVATE
	# FIXME(strager): This is gross.
	-Wall -Wextra -O2 -g
	$<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions -fno-rtti>
)
target_include_directories(gambatte-testrunner PRIVATE ../common)
target_link_libraries(gambatte-testrunner PRIVATE PNG::PNG gambatte)

add_subdirectory(api)
add_subdirectory(hwtests)
