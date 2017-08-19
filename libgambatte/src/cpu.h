//
//   Copyright (C) 2007 by sinamas <sinamas at users.sourceforge.net>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License version 2 for more details.
//
//   You should have received a copy of the GNU General Public License
//   version 2 along with this program; if not, write to the
//   Free Software Foundation, Inc.,
//   51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA.
//

#ifndef CPU_H
#define CPU_H

#include "breakpoints.h"
#include "memory.h"
#include "stopinfo.h"

namespace gambatte {

class CPU {
public:
	CPU();
	StopInfo runFor(unsigned long cycles);
	void setStatePtrs(SaveState &state);
	void saveState(SaveState &state);
	void loadState(SaveState const &state);
	bool loadSavedata(std::istream * saveFile, std::istream * rtcFile) { return mem_.loadSavedata(saveFile, rtcFile); }
	bool saveSavedata(std::ostream * saveFile, std::ostream * rtcFile) const { return mem_.saveSavedata(saveFile, rtcFile); }

	void setVideoBuffer(uint_least32_t *videoBuf, std::ptrdiff_t pitch) {
		mem_.setVideoBuffer(videoBuf, pitch);
	}

	void setInput(unsigned char input) {
		mem_.setInput(input);
	}

	LoadRes load(std::string const &romfile, bool forceDmg, bool multicartCompat) {
		LoadRes res = mem_.loadROM(romfile, forceDmg, multicartCompat);
		breakpoints_.reloadROMBreakpoints(mem_.cart());
		return res;
	}

	bool loaded() const { return mem_.loaded(); }
	char const * romTitle() const { return mem_.romTitle(); }
	PakInfo const pakInfo(bool multicartCompat) const { return mem_.pakInfo(multicartCompat); }
	void setSoundBuffer(uint_least32_t *buf) { mem_.setSoundBuffer(buf); }
	std::size_t fillSoundBuffer() { return mem_.fillSoundBuffer(cycleCounter_); }
	bool isCgb() const { return mem_.isCgb(); }

	void setDmgPaletteColor(int palNum, int colorNum, unsigned long rgb32) {
		mem_.setDmgPaletteColor(palNum, colorNum, rgb32);
	}

	void setGameGenie(std::string const &codes) { mem_.setGameGenie(codes); }
	void setGameShark(std::string const &codes) { mem_.setGameShark(codes); }

	bool addROMBreakpoint(std::uint_least32_t fileOffset);
	bool removeROMBreakpoint(std::uint_least32_t fileOffset);
	void clearROMBreakpoints();

private:
	Memory mem_;
	unsigned long cycleCounter_;
	unsigned short pc_;
	unsigned short sp;
	unsigned hf1, hf2, zf, cf;
	unsigned char a_, b, c, d, e, /*f,*/ h, l;
	bool skip_;
	Breakpoints breakpoints_;
};

}

#endif
