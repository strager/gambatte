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

#ifndef MEMORY_H
#define MEMORY_H

#include "mem/cartridge.h"
#include "interrupter.h"
#include "pakinfo.h"
#include "sound.h"
#include "tima.h"
#include "video.h"
#include <iosfwd>

namespace gambatte {

class FilterInfo;

class Memory {
public:
	enum Button { A     = 0x01, B    = 0x02, SELECT = 0x04, START = 0x08,
	              RIGHT = 0x10, LEFT = 0x20, UP     = 0x40, DOWN  = 0x80 };

	explicit Memory(Interrupter const &interrupter);
	Cartridge const &cart() const { return cart_; }
	bool loaded() const { return cart_.loaded(); }
	char const * romTitle() const { return cart_.romTitle(); }
	PakInfo const pakInfo(bool multicartCompat) const { return cart_.pakInfo(multicartCompat); }
	void setStatePtrs(SaveState &state);
	unsigned long saveState(SaveState &state, unsigned long cc);
	void loadState(SaveState const &state);
	bool loadSavedata(std::istream * saveFile, std::istream * rtcFile) { return cart_.loadSavedata(saveFile, rtcFile); }
	bool saveSavedata(std::ostream * saveFile, std::ostream * rtcFile) const { return cart_.saveSavedata(saveFile, rtcFile); }

	unsigned long stop(unsigned long cycleCounter);
	bool isCgb() const { return lcd_.isCgb(); }
	bool ime() const { return intreq_.ime(); }
	bool halted() const { return intreq_.halted(); }
	unsigned long nextEventTime() const { return intreq_.minEventTime(); }
	bool isActive() const { return intreq_.eventTime(intevent_end) != disabled_time; }

	long cyclesSinceBlit(unsigned long cc) const {
		if (cc < intreq_.eventTime(intevent_blit))
			return -1;

		return (cc - intreq_.eventTime(intevent_blit)) >> isDoubleSpeed();
	}

	void halt() { intreq_.halt(); }
	void ei(unsigned long cycleCounter) { if (!ime()) { intreq_.ei(cycleCounter); } }
	void di() { intreq_.di(); }

	unsigned ff_read(unsigned p, unsigned long cc) {
		return p < 0x80 ? nontrivial_ff_read(p, cc) : ioamhram_[p + 0x100];
	}

	unsigned read(unsigned p, unsigned long cc) {
		return cart_.rmem(p >> 12) ? cart_.rmem(p >> 12)[p] : nontrivial_read(p, cc);
	}

	void externalRead(uint_least16_t p, unsigned char * data, std::size_t size, unsigned long const cc) const;

	void write(unsigned p, unsigned data, unsigned long cc) {
		if (cart_.wmem(p >> 12)) {
			cart_.wmem(p >> 12)[p] = data;
		} else
			nontrivial_write(p, data, cc);
	}

	void ff_write(unsigned p, unsigned data, unsigned long cc) {
		if (p - 0x80u < 0x7Fu) {
			ioamhram_[p + 0x100] = data;
		} else
			nontrivial_ff_write(p, data, cc);
	}

	unsigned long event(unsigned long cycleCounter);
	unsigned long resetCounters(unsigned long cycleCounter);
	LoadRes loadROM(std::string const &romfile, bool forceDmg, bool multicartCompat);
	void setInput(unsigned char input) { input_ = input; }
	void setEndtime(unsigned long cc, unsigned long inc);
	void setSoundBuffer(uint_least32_t *buf) { psg_.setBuffer(buf); }
	std::size_t fillSoundBuffer(unsigned long cc);

	void setVideoBuffer(uint_least32_t *videoBuf, std::ptrdiff_t pitch) {
		lcd_.setVideoBuffer(videoBuf, pitch);
	}

	void setDmgPaletteColor(int palNum, int colorNum, unsigned long rgb32) {
		lcd_.setDmgPaletteColor(palNum, colorNum, rgb32);
	}

	void setGameGenie(std::string const &codes) { cart_.setGameGenie(codes); }
	void setGameShark(std::string const &codes) { interrupter_.setGameShark(codes); }
	void updateInput();

private:
	Cartridge cart_;
	unsigned char ioamhram_[0x200];
	unsigned long divLastUpdate_;
	unsigned long lastOamDmaUpdate_;
	InterruptRequester intreq_;
	Tima tima_;
	LCD lcd_;
	PSG psg_;
	Interrupter interrupter_;
	unsigned short dmaSource_;
	unsigned short dmaDestination_;
	unsigned char oamDmaPos_;
	unsigned char serialCnt_;
	bool blanklcd_;
	unsigned char input_;

	void decEventCycles(IntEventId eventId, unsigned long dec);
	void oamDmaInitSetup();
	void updateOamDma(unsigned long cycleCounter);
	void startOamDma(unsigned long cycleCounter);
	void endOamDma(unsigned long cycleCounter);
	unsigned char const * oamDmaSrcPtr() const;
	unsigned nontrivial_ff_read(unsigned p, unsigned long cycleCounter);
	unsigned nontrivial_read(unsigned p, unsigned long cycleCounter);
	void nontrivial_ff_write(unsigned p, unsigned data, unsigned long cycleCounter);
	void nontrivial_write(unsigned p, unsigned data, unsigned long cycleCounter);
	void updateSerial(unsigned long cc);
	void updateTimaIrq(unsigned long cc);
	void updateIrqs(unsigned long cc);
	bool isDoubleSpeed() const { return lcd_.isDoubleSpeed(); }
};

}

#endif
