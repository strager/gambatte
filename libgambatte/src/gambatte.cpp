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

#include "gambatte.h"
#include "cpu.h"
#include "initstate.h"
#include "savestate.h"
#include "statesaver.h"
#include <cstring>
#include <sstream>

namespace gambatte {

struct GB::Priv {
	CPU cpu;
	unsigned loadflags;

	Priv() : loadflags(0) {}
};

GB::GB() : p_(new Priv) {}

GB::~GB() {
	delete p_;
}

void GB::setVideoBuffer(gambatte::uint_least32_t *videoBuf, std::ptrdiff_t pitch) {
	p_->cpu.setVideoBuffer(videoBuf, pitch);
}

StopInfo GB::runFor(gambatte::uint_least32_t *const soundBuf, std::size_t samples) {
	if (!p_->cpu.loaded()) {
		StopInfo stopInfo;
		stopInfo.stopReason = StopInfo::ERROR_NOT_LOADED;
		return stopInfo;
	}

	p_->cpu.setSoundBuffer(soundBuf);

	return p_->cpu.runFor(samples * 2);
}

void GB::reset() {
	if (p_->cpu.loaded()) {
		std::stringstream saveFile;
		std::stringstream rtcFile;
		p_->cpu.saveSavedata(&saveFile, &rtcFile);
		saveFile.seekg(0);
		rtcFile.seekg(0);

		SaveState state;
		p_->cpu.setStatePtrs(state);
		setInitState(state, p_->cpu.isCgb(), p_->loadflags & GBA_CGB);
		p_->cpu.loadState(state);
		p_->cpu.loadSavedata(&saveFile, &rtcFile);
	}
}

void GB::setInput(unsigned char input) {
	p_->cpu.setInput(input);
}

LoadRes GB::load(std::string const &romfile, unsigned const flags) {
	LoadRes const loadres = p_->cpu.load(romfile,
	                                     flags & FORCE_DMG,
	                                     flags & MULTICART_COMPAT);
	if (loadres == LOADRES_OK) {
		SaveState state;
		p_->cpu.setStatePtrs(state);
		p_->loadflags = flags;
		setInitState(state, p_->cpu.isCgb(), flags & GBA_CGB);
		p_->cpu.loadState(state);
	}

	return loadres;
}

bool GB::isCgb() const {
	return p_->cpu.isCgb();
}

bool GB::isLoaded() const {
	return p_->cpu.loaded();
}

bool GB::loadSavedata(std::istream * saveFile, std::istream * rtcFile) {
	if (!p_->cpu.loaded())
		return false;
	return p_->cpu.loadSavedata(saveFile, rtcFile);
}

bool GB::saveSavedata(std::ostream * saveFile, std::ostream * rtcFile) const {
	if (!p_->cpu.loaded())
		return false;
	return p_->cpu.saveSavedata(saveFile, rtcFile);
}

void GB::setDmgPaletteColor(int palNum, int colorNum, unsigned long rgb32) {
	p_->cpu.setDmgPaletteColor(palNum, colorNum, rgb32);
}

bool GB::loadState(std::istream &file) {
	if (p_->cpu.loaded()) {
		SaveState state;
		p_->cpu.setStatePtrs(state);

		if (StateSaver::loadState(state, file)) {
			p_->cpu.loadState(state);
			return true;
		}
	}

	return false;
}

bool GB::saveState(gambatte::uint_least32_t const *videoBuf, std::ptrdiff_t pitch,
                   std::ostream &file) {
	if (p_->cpu.loaded()) {
		SaveState state;
		p_->cpu.setStatePtrs(state);
		p_->cpu.saveState(state);
		return StateSaver::saveState(state, videoBuf, pitch, file);
	}

	return false;
}

std::string const GB::romTitle() const {
	if (p_->cpu.loaded()) {
		char title[0x11];
		std::memcpy(title, p_->cpu.romTitle(), 0x10);
		title[title[0xF] & 0x80 ? 0xF : 0x10] = '\0';
		return std::string(title);
	}

	return std::string();
}

PakInfo const GB::pakInfo() const { return p_->cpu.pakInfo(p_->loadflags & MULTICART_COMPAT); }

void GB::setGameGenie(std::string const &codes) {
	p_->cpu.setGameGenie(codes);
}

void GB::setGameShark(std::string const &codes) {
	p_->cpu.setGameShark(codes);
}

bool GB::addROMBreakpoint(uint_least32_t fileOffset) {
	return p_->cpu.addROMBreakpoint(fileOffset);
}

bool GB::removeROMBreakpoint(uint_least32_t fileOffset) {
	return p_->cpu.removeROMBreakpoint(fileOffset);
}

void GB::clearROMBreakpoints() {
	p_->cpu.clearROMBreakpoints();
}

}
