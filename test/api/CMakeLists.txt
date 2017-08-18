cmake_minimum_required(VERSION 3.5)

project(gambatte-test-api)

include(Gameboy)

add_executable(gambatte-api-test_savedata test_savedata.cpp)
target_link_libraries(gambatte-api-test_savedata PRIVATE gambatte)

add_gameboy_rom(gambatte-api-modify_sram modify_sram.asm)
add_test(
	NAME
	gambatte-api-test_savedata
	COMMAND
	gambatte-api-test_savedata
	$<TARGET_PROPERTY:gambatte-api-modify_sram,TARGET_FILE>
)
