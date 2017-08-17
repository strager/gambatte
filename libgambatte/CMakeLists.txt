cmake_minimum_required(VERSION 3.5)

project(libgambatte)

find_package(ZLIB MODULE)

set(
	SOURCES
	src/bitmap_font.cpp
	src/cpu.cpp
	src/gambatte.cpp
	src/initstate.cpp
	src/interrupter.cpp
	src/interruptrequester.cpp
	src/loadres.cpp
	src/mem/cartridge.cpp
	src/mem/memptrs.cpp
	src/mem/pakinfo.cpp
	src/mem/rtc.cpp
	src/memory.cpp
	src/sound.cpp
	src/sound/channel1.cpp
	src/sound/channel2.cpp
	src/sound/channel3.cpp
	src/sound/channel4.cpp
	src/sound/duty_unit.cpp
	src/sound/envelope_unit.cpp
	src/sound/length_counter.cpp
	src/state_osd_elements.cpp
	src/statesaver.cpp
	src/tima.cpp
	src/video.cpp
	src/video/ly_counter.cpp
	src/video/lyc_irq.cpp
	src/video/next_m0_time.cpp
	src/video/ppu.cpp
	src/video/sprite_mapper.cpp
)
if (TARGET ZLIB::ZLIB)
	list(
		APPEND
		SOURCES
		src/file/file_zip.cpp
		src/file/unzip/ioapi.c
		src/file/unzip/unzip.c
	)
else ()
	list(APPEND SOURCES src/file/file.cpp)
endif ()

add_library(gambatte ${SOURCES})
target_compile_definitions(gambatte PUBLIC HAVE_STDINT_H)
target_compile_options(
	gambatte
	PRIVATE
	# FIXME(strager): This is gross.
	-Wall -Wextra -O2 -fomit-frame-pointer
	$<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions -fno-rtti>
)
target_include_directories(gambatte PRIVATE ../common src)
target_include_directories(gambatte PUBLIC include)
if (TARGET ZLIB::ZLIB)
	target_link_libraries(gambatte PRIVATE ZLIB::ZLIB)
endif ()
