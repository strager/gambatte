#include "gambatte.h"
#include <cstddef>
#include <cstdio>
#include <sstream>

namespace {

enum { SAMPLES_PER_FRAME = 35112, EXTRA_SAMPLES = 2064 };
enum { SCREEN_WIDTH = 160, SCREEN_HEIGHT = 144 };

void runUntilNextFrame(gambatte::GB &gb) {
	for (;;) {
		gambatte::uint_least32_t videoBuf[SCREEN_WIDTH * SCREEN_HEIGHT];
		gambatte::uint_least32_t soundBuf[SAMPLES_PER_FRAME + EXTRA_SAMPLES];
		std::size_t samples = sizeof(soundBuf) / sizeof(*soundBuf);
		std::ptrdiff_t result = gb.runFor(videoBuf, SCREEN_WIDTH, soundBuf, samples);
		if (result >= 0) {
			break;
		}
	}
}

bool testSavingIsIdempotent(char const * romPath) {
	gambatte::GB gb;
	gambatte::LoadRes loadResult = gb.load(romPath, gambatte::GB::GBA_CGB);
	if (loadResult != gambatte::LOADRES_OK) {
		std::fprintf(stderr, "error: failed to load %s\n", romPath);
		return false;
	}

	enum { FRAMES_TO_TEST = 10 };
	for (int i = 0; i < FRAMES_TO_TEST; ++i) {
		bool ok;
		// Save twice and ensure both saveStates are bit-identical.
		std::stringstream saveStates[2];
		ok = gb.saveState(NULL, 0, saveStates[0]);
		if (!ok) {
			std::fprintf(stderr, "error: saving state failed\n");
			return false;
		}
		ok = gb.saveState(NULL, 0, saveStates[1]);
		if (!ok) {
			std::fprintf(stderr, "error: saving state failed\n");
			return false;
		}
		saveStates[0].seekg(0);
		saveStates[1].seekg(0);
		if (saveStates[0].str() != saveStates[1].str()) {
			std::fprintf(stderr, "error: save states were not identical\n");
		}

		runUntilNextFrame(gb);
		// TODO(strager): Check correctness.
	}
	return true;
}

bool testRewind(char const * romPath) {
	gambatte::GB gb;
	gambatte::LoadRes loadResult = gb.load(romPath, gambatte::GB::GBA_CGB);
	if (loadResult != gambatte::LOADRES_OK) {
		std::fprintf(stderr, "error: failed to load %s\n", romPath);
		return false;
	}

	enum { FRAMES_TO_TEST = 10 };

	// Capture states for each frame.
	std::stringstream saveStates[FRAMES_TO_TEST];
	for (int i = 0; i < FRAMES_TO_TEST; ++i) {
		bool ok = gb.saveState(NULL, 0, saveStates[i]);
		if (!ok) {
			std::fprintf(stderr, "error: saving state failed\n");
			return false;
		}

		runUntilNextFrame(gb);
		// TODO(strager): Check correctness.
	}

	// Load states in backwards order. Capture the state for the current
	// frame and the following frame. Ensure those two frames have the same
	// states as saved previously.
	for (int i = FRAMES_TO_TEST - 1; i >= 0; ++i) {
		bool ok;
		saveStates[i].seekg(0);
		ok = gb.loadState(saveStates[i]);
		if (!ok) {
			std::fprintf(stderr, "error: loading state failed\n");
			return false;
		}

		for (int j = i; j < i + 2 && j < FRAMES_TO_TEST; ++j) {
			std::stringstream curSaveState;
			ok = gb.saveState(NULL, 0, curSaveState);
			if (!ok) {
				std::fprintf(stderr, "error: saving state failed\n");
				return false;
			}
			saveStates[j].seekg(0);
			curSaveState.seekg(0);
			if (saveStates[j].str() != curSaveState.str()) {
				std::fprintf(stderr, "error: save state were not identical\n");
				return false;
			}

			runUntilNextFrame(gb);
			// TODO(strager): Check correctness.
		}
	}
	return true;
}

bool test(char const * romPath) {
	return testSavingIsIdempotent(romPath) && testRewind(romPath);
}

}

int main(int argc, char **argv) {
}
