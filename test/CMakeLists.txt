cmake_minimum_required(VERSION 3.5)

project(gambatte-test)

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

file(
	GLOB_RECURSE
	HWTEST_SOURCES
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	hwtests/*.asm
)
foreach (HWTEST_SOURCE ${HWTEST_SOURCES})
	get_filename_component(HWTEST_NAME "${HWTEST_SOURCE}" NAME_WE)
	get_filename_component(HWTEST_DIRECTORY "${HWTEST_SOURCE}" DIRECTORY)

	set(HWTEST_FULL_NAME "${HWTEST_DIRECTORY}/${HWTEST_NAME}")
	string(REGEX REPLACE / - HWTEST_FULL_NAME "${HWTEST_FULL_NAME}")

	# Is this hwtest for GBC or GB?
	set(HWTEST_GBC_REGEX "^[ ]*\\.data[ ]*@[ ]*143[ ]*$")
	file(STRINGS "${HWTEST_SOURCE}" HWTEST_GBC_MARKERS LIMIT_COUNT 1 REGEX "${HWTEST_GBC_REGEX}")
	if ("${HWTEST_GBC_MARKERS}" STREQUAL "")
		set(HWTEST_IS_GBC FALSE)
	else ()
		set(HWTEST_IS_GBC TRUE)
	endif ()
	# TODO(strager): Make qdgbas support output files and compile the
	# .gb/.gbc into the build directory.
	set(HWTEST_ROM "${CMAKE_CURRENT_SOURCE_DIR}/${HWTEST_DIRECTORY}/${HWTEST_NAME}")
	if (HWTEST_IS_GBC)
		set(HWTEST_ROM "${HWTEST_ROM}.gbc")
	else ()
		set(HWTEST_ROM "${HWTEST_ROM}.gb")
	endif ()

	add_custom_command(
		OUTPUT "${HWTEST_ROM}"
		COMMAND
		python
		"${CMAKE_CURRENT_SOURCE_DIR}/qdgbas.py"
		"${CMAKE_CURRENT_SOURCE_DIR}/${HWTEST_SOURCE}"
		MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${HWTEST_SOURCE}"
		DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/qdgbas.py"
		COMMENT "Building Game Boy ROM ${HWTEST_ROM}"
	)
	add_custom_target(
		gambatte-hwtest-${HWTEST_FULL_NAME}
		ALL
		DEPENDS "${HWTEST_ROM}"
		SOURCES "${HWTEST_SOURCE}"
	)
	add_test(
		NAME
		"gambatte-hwtest-${HWTEST_FULL_NAME}"
		COMMAND
		gambatte-testrunner "${HWTEST_ROM}"
	)
endforeach ()
