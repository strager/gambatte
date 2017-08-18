#include "gambatte.h"
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <sstream>

namespace {

enum { SAMPLES_PER_FRAME = 35112, EXTRA_SAMPLES = 2064 };
enum { SCREEN_WIDTH = 160, SCREEN_HEIGHT = 144 };

void runUntilNextFrame(gambatte::GB &gb) {
	for (;;) {
		gambatte::uint_least32_t soundBuf[SAMPLES_PER_FRAME + EXTRA_SAMPLES];
		std::size_t samples = sizeof(soundBuf) / sizeof(*soundBuf);
		gambatte::StopInfo stopInfo = gb.runFor(soundBuf, samples);
		switch (stopInfo.stopReason) {
		case gambatte::StopInfo::REQUESTED_SAMPLES_PRODUCED:
			break;
		case gambatte::StopInfo::VIDEO_FRAME_PRODUCED:
			return;
		default:
			assert(false);
			return;
		}
	}
}

bool testEmptySaveSavedata(char const * romPath) {
	gambatte::GB gb;
	gambatte::LoadRes loadResult = gb.load(romPath, gambatte::GB::GBA_CGB);
	if (loadResult != gambatte::LOADRES_OK) {
		std::fprintf(stderr, "error: failed to load %s\n", romPath);
		return false;
	}

	runUntilNextFrame(gb);

	std::stringstream saveFile;
	bool ok = gb.saveSavedata(&saveFile, NULL);
	if (!ok) {
		std::fprintf(stderr, "error: failed to save savedata\n");
		return false;
	}
	std::string bytes = saveFile.str();
	if (bytes.size() != 32768) {
		std::fprintf(stderr, "error: expected savedata to have %d bytes, got %zu bytes\n", 32768, bytes.size());
		return false;
	}
	if (bytes[0x0000] != 0x04) {
		std::fprintf(stderr, "error: for byte %#04x, expected %#02x, got %#02x\n", 0x7FFF, 0x04, bytes[0x0000]);
		return false;
	}
	for (int i = 0x0001; i < 0x7FFF; ++i) {
		if (static_cast<unsigned char>(bytes[i]) != 0xFF) {
			std::fprintf(stderr, "error: for byte %#04x, expected %#02x, got %#02x\n", i, 0xFF, bytes[i]);
			return false;
		}
	}
	if (static_cast<unsigned char>(bytes[0x7FFF]) != 0xF6) {
		std::fprintf(stderr, "error: for byte %#04x, expected %#02x, got %#02x\n", 0x7FFF, 0xF6, bytes[0x7FFF]);
		return false;
	}

	return true;
}

bool testLoadSaveSavedata(char const * romPath) {
	bool ok;

	gambatte::GB gb;
	gambatte::LoadRes loadResult = gb.load(romPath, gambatte::GB::GBA_CGB);
	if (loadResult != gambatte::LOADRES_OK) {
		std::fprintf(stderr, "error: failed to load %s\n", romPath);
		return false;
	}

	std::stringstream originalSaveFile;
	originalSaveFile.put(0x55);
	for (long i = 0x0001; i < 0x7FFF; ++i) {
		originalSaveFile.put((i * 10) % 0x67);
	}
	originalSaveFile.put(0x07);
	originalSaveFile.seekg(0);
	ok = gb.loadSavedata(&originalSaveFile, NULL);
	if (!ok) {
		std::fprintf(stderr, "error: failed to load savedata\n");
		return false;
	}

	runUntilNextFrame(gb);

	std::stringstream saveFile;
	ok = gb.saveSavedata(&saveFile, NULL);
	std::string bytes = saveFile.str();
	std::string originalBytes = originalSaveFile.str();
	if (bytes.size() != 32768) {
		std::fprintf(stderr, "error: expected savedata to have %d bytes, got %zu bytes\n", 32768, bytes.size());
		return false;
	}
	if (bytes[0x0000] != 0x5A) {
		std::fprintf(stderr, "error: for byte %#04x, expected %#02x, got %#02x\n", 0x7FFF, 0x5A, bytes[0x0000]);
		return false;
	}
	for (int i = 0x0001; i < 0x7FFF; ++i) {
		if (bytes[i] != originalBytes[i]) {
			std::fprintf(stderr, "error: for byte %#04x, expected %#02x, got %#02x\n", i, originalBytes[i], bytes[i]);
			return false;
		}
	}
	if (static_cast<unsigned char>(bytes[0x7FFF]) != 0xFE) {
		std::fprintf(stderr, "error: for byte %#04x, expected %#02x, got %#02x\n", 0x7FFF, 0xFE, bytes[0x7FFF]);
		return false;
	}

	return true;
}

bool test(char const * romPath) {
	return testEmptySaveSavedata(romPath) && testLoadSaveSavedata(romPath);
}

}

int main(int argc, char ** argv) {
	if (argc != 2) {
		std::fprintf(stderr, "error: expected one argument\n");
		return EXIT_FAILURE;
	}
	char const * romPath = argv[1];
	return test(romPath) ? EXIT_SUCCESS : EXIT_FAILURE;
}
