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

add_executable(gambatte-api-test_save_state test_save_state.cpp)
target_link_libraries(gambatte-api-test_save_state PRIVATE gambatte)

add_gameboy_rom(gambatte-api-palette_cycle palette_cycle.asm)
add_test(
	NAME
	gambatte-api-test_save_state
	COMMAND
	gambatte-api-test_save_state
	$<TARGET_PROPERTY:gambatte-api-palette_cycle,TARGET_FILE>
)
