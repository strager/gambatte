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

#include "breakpoints.h"
#include "mem/cartridge.h"
#include <cassert>
#include <utility>

namespace gambatte {

Breakpoints::Breakpoints() {
	clearROMBreakpoints();
}

bool Breakpoints::addROMBreakpoint(Cartridge const &cart, std::uint_least32_t fileOffset) {
	int count = romBreakpointCount();
	for (int i = 0; i < count; ++i) {
		if (romBreakpointOffsets_[i] == fileOffset)
			return false;
	}
	if (count == MAX_ROM_BREAKPOINTS)
		return false;
	unsigned char const * romdata = cart.romdata();
	// TODO(strager): Check bounds.
	unsigned char const * byte = &romdata[fileOffset];
	assert(byte);
	romBreakpointOffsets_[count] = fileOffset;
	romBreakpointBytes_[count] = byte;
	return true;
}

bool Breakpoints::removeROMBreakpoint(std::uint_least32_t fileOffset) {
	int count = romBreakpointCount();
	for (int i = 0; i < count; ++i) {
		if (romBreakpointOffsets_[i] == fileOffset) {
			if (i != count - 1) {
				using std::swap;
				swap(romBreakpointBytes_[i], romBreakpointBytes_[count - 1]);
				swap(romBreakpointOffsets_[i], romBreakpointOffsets_[count - 1]);
			}
			return true;
		}
	}
	return false;
}

void Breakpoints::clearROMBreakpoints() {
	romBreakpointBytes_[0] = NULL;
}

void Breakpoints::reloadROMBreakpoints(Cartridge const &) {
	// TODO(strager)
	assert(!hasROMBreakpoints());
}

bool Breakpoints::hasROMBreakpoints() const {
	return romBreakpointBytes_[0];
}

bool Breakpoints::testROMBreakpoint(unsigned char const * romByte, gambatte::uint_least32_t * outFileOffset) const {
	for (int i = 0; i < MAX_ROM_BREAKPOINTS && romBreakpointBytes_[i]; ++i) {
		if (romBreakpointBytes_[i] == romByte) {
			if (outFileOffset)
				*outFileOffset = romBreakpointOffsets_[i];
			return true;
		}
	}
	return false;
}

int Breakpoints::romBreakpointCount() const {
	int i;
	for (i = 0; i < MAX_ROM_BREAKPOINTS && romBreakpointBytes_[i]; ++i)
		; // Count.
	return i;
}

}
