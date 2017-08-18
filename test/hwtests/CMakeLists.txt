cmake_minimum_required(VERSION 3.5)

project(gambatte-test-hwtests)

include(Gameboy)

file(
	GLOB_RECURSE
	HWTEST_SOURCES
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	*.asm
)
foreach (HWTEST_SOURCE ${HWTEST_SOURCES})
	get_filename_component(HWTEST_FILE_NAME "${HWTEST_SOURCE}" NAME_WE)
	get_filename_component(HWTEST_DIRECTORY "${HWTEST_SOURCE}" DIRECTORY)

	set(HWTEST_NAME "gambatte-hwtest-${HWTEST_DIRECTORY}/${HWTEST_FILE_NAME}")
	string(REGEX REPLACE / - HWTEST_NAME "${HWTEST_NAME}")

	add_gameboy_rom("${HWTEST_NAME}" "${HWTEST_SOURCE}")
	add_test(
		NAME
		"${HWTEST_NAME}"
		COMMAND
		gambatte-testrunner
		"$<TARGET_PROPERTY:${HWTEST_NAME},TARGET_FILE>"
	)
endforeach ()
