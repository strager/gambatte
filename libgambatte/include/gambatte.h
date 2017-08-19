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

#ifndef GAMBATTE_H
#define GAMBATTE_H

#include "gbint.h"
#include "loadres.h"
#include "stopinfo.h"
#include <cstddef>
#include <iosfwd>
#include <string>

namespace gambatte {

enum { BG_PALETTE = 0, SP1_PALETTE = 1, SP2_PALETTE = 2 };

class GB {
public:
	GB();
	~GB();

	enum LoadFlag {
		FORCE_DMG        = 1, /**< Treat the ROM as not having CGB support regardless of
		                           what its header advertises. */
		GBA_CGB          = 2, /**< Use GBA intial CPU register values when in CGB mode. */
		MULTICART_COMPAT = 4  /**< Use heuristics to detect and support some multicart
		                           MBCs disguised as MBC1. */
	};

	enum Button { A     = 0x01, B    = 0x02, SELECT = 0x04, START = 0x08,
	              RIGHT = 0x10, LEFT = 0x20, UP     = 0x40, DOWN  = 0x80 };

	 /*
	  * Load ROM image.
	  *
	  * @param romfile  Path to rom image file. Typically a .gbc, .gb, or .zip-file (if
	  *                 zip-support is compiled in).
	  * @param flags    ORed combination of LoadFlags.
	  * @return 0 on success, negative value on failure.
	  */
	LoadRes load(std::string const &romfile, unsigned flags = 0);

	/**
	  * @param videoBuf 160x144 RGB32 (native endian) video frame buffer or 0
	  * @param pitch distance in number of pixels (not bytes) from the start of one line
	  *              to the next in videoBuf.
	  */
	void setVideoBuffer(gambatte::uint_least32_t *videoBuf, std::ptrdiff_t pitch);

	/**
	  * Emulates until at least 'samples' audio samples are produced in the
	  * supplied audio buffer, or until a video frame has been drawn.
	  *
	  * There are 35112 audio (stereo) samples in a video frame.
	  * May run for up to 2064 audio samples too long.
	  *
	  * An audio sample consists of two native endian 2s complement 16-bit PCM samples,
	  * with the left sample preceding the right one. Usually casting audioBuf to
	  * int16_t* is OK. The reason for using an uint_least32_t* in the interface is to
	  * avoid implementation-defined behavior without compromising performance.
	  * libgambatte is strictly c++98, so fixed-width types are not an option (and even
	  * c99/c++11 cannot guarantee their availability).
	  *
	  * Returns early when a new video frame has finished drawing in the video buffer,
	  * such that the caller may update the video output before the frame is overwritten.
	  * In that case, StopInfo::stopReason indicates whether a new video
	  * frame has been drawn, and StopInfo::videoFrameSampleOffset indicates
	  * the exact time (in number of samples) at which it was completed.
	  *
	  * @param audioBuf buffer with space >= samples + 2064
	  * @param samples number of stereo samples to produce
	  */
	StopInfo runFor(gambatte::uint_least32_t *audioBuf, std::size_t samples);

	/**
	  * Reset to initial state.
	  * Equivalent to reloading a ROM image, or turning a Game Boy Color off and on again.
	  */
	void reset();

	/**
	  * @param palNum 0 <= palNum < 3. One of BG_PALETTE, SP1_PALETTE and SP2_PALETTE.
	  * @param colorNum 0 <= colorNum < 4
	  */
	void setDmgPaletteColor(int palNum, int colorNum, unsigned long rgb32);

	/** Sets what buttons are pressed. */
	void setInput(unsigned char input);

	/**
	  * Sets the directory used for storing save data. The default is the same directory as
	  * the ROM Image file.
	  */
	void setSaveDir(std::string const &sdir);

	/** Returns true if the currently loaded ROM image is treated as having CGB support. */
	bool isCgb() const;

	/** Returns true if a ROM image is loaded. */
	bool isLoaded() const;

	/** Reads persistent cartridge data from disk. */
	bool loadSavedata(std::istream * saveFile, std::istream * rtcFile);

	/** Writes persistent cartridge data to disk. */
	bool saveSavedata(std::ostream * saveFile, std::ostream * rtcFile) const;

	/**
	  * Saves emulator state to the file given by 'file'.
	  *
	  * @param  videoBuf 160x144 RGB32 (native endian) video frame buffer or 0. Used for
	  *                  saving a thumbnail.
	  * @param  pitch distance in number of pixels (not bytes) from the start of one line
	  *               to the next in videoBuf.
	  * @return success
	  */
	bool saveState(gambatte::uint_least32_t const *videoBuf, std::ptrdiff_t pitch,
	               std::ostream &file);

	/**
	  * Loads emulator state from the file given by 'file'.
	  * @return success
	  */
	bool loadState(std::istream &file);

	/** ROM header title of currently loaded ROM image. */
	std::string const romTitle() const;

	/** GamePak/Cartridge info. */
	class PakInfo const pakInfo() const;

	/**
	  * Set Game Genie codes to apply to currently loaded ROM image. Cleared on ROM load.
	  * @param codes Game Genie codes in format HHH-HHH-HHH;HHH-HHH-HHH;... where
	  *              H is [0-9]|[A-F]
	  */
	void setGameGenie(std::string const &codes);

	/**
	  * Set Game Shark codes to apply to currently loaded ROM image. Cleared on ROM load.
	  * @param codes Game Shark codes in format 01HHHHHH;01HHHHHH;... where H is [0-9]|[A-F]
	  */
	void setGameShark(std::string const &codes);

	bool addROMBreakpoint(uint_least32_t fileOffset);

	bool removeROMBreakpoint(uint_least32_t fileOffset);

	void clearROMBreakpoints();

	void readMemory(uint_least16_t address, unsigned char * data, std::size_t size) const;

private:
	struct Priv;
	Priv *const p_;

	GB(GB const &);
	GB & operator=(GB const &);
};

}

#endif
