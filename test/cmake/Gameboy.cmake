set(GAMBATTE_QDGBAS "" CACHE PATH "Path to the Quick and Dirty Game Boy ASsembler")

function (add_gameboy_rom NAME)
	if (NOT GAMBATTE_QDGBAS)
		message(FATAL_ERROR "Missing required GAMBATTE_QDGBAS")
		return ()
	endif ()

	cmake_parse_arguments("" "" "" "" ${ARGN})
	set(SOURCES ${_UNPARSED_ARGUMENTS})
	list(LENGTH SOURCES SOURCE_COUNT)
	if (SOURCE_COUNT GREATER 1)
		message(
			AUTHOR_WARNING
			"add_gameboy_rom accepts only one source"
		)
		list(GET SOURCES 0 SOURCE)
	else ()
		set(SOURCE ${SOURCES})
	endif ()
	get_filename_component(
		SOURCE
		"${SOURCE}"
		ABSOLUTE
		BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
	)

	# Is this ROM for GBC or GB?
	set(GBC_REGEX "^[ ]*\\.data[ ]*@[ ]*143[ ]*$")
	file(STRINGS "${SOURCE}" GBC_MARKERS LIMIT_COUNT 1 REGEX "${GBC_REGEX}")
	if ("${GBC_MARKERS}" STREQUAL "")
		set(IS_GBC FALSE)
	else ()
		set(IS_GBC TRUE)
	endif ()

	# TODO(strager): Make qdgbas support output files and compile the
	# .gb/.gbc into the build directory.
	get_filename_component(SOURCE_NAME_WE "${SOURCE}" NAME_WE)
	get_filename_component(SOURCE_DIRECTORY "${SOURCE}" DIRECTORY)
	set(ROM "${SOURCE_DIRECTORY}/${SOURCE_NAME_WE}")
	if (IS_GBC)
		set(ROM "${ROM}.gbc")
	else ()
		set(ROM "${ROM}.gb")
	endif ()

	add_custom_command(
		OUTPUT "${ROM}"
		COMMAND
		python
		"${GAMBATTE_QDGBAS}"
		"${SOURCE}"
		MAIN_DEPENDENCY "${SOURCE}"
		DEPENDS "${GAMBATTE_QDGBAS}"
		COMMENT "Building Game Boy ROM ${ROM}"
	)
	add_custom_target(
		"${NAME}"
		ALL
		DEPENDS "${ROM}"
		SOURCES "${SOURCE}"
	)
	set_target_properties(
		"${NAME}"
		PROPERTIES
		IS_GBC "${IS_GBC}"
		TARGET_FILE "${ROM}"
	)
endfunction ()
