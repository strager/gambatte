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

#ifndef GAMBATTE_STOPINFO_H
#define GAMBATTE_STOPINFO_H

namespace gambatte {

struct StopInfo {
	enum StopReason {
		ERROR_NOT_LOADED = 1,
		/**
		  * GB::runFor() produced at least the number of samples
		  * requested.
		  */
		REQUESTED_SAMPLES_PRODUCED = 2,
		VIDEO_FRAME_PRODUCED = 3,
	};

	StopReason stopReason;

	/**
	  * Set iff stopReason == REQUESTED_SAMPLES_PRODUCED || stopReason ==
	  * VIDEO_FRAME_PRODUCED.
	  */
	std::size_t samplesProduced;

	/**
	  * Sample offset in the sound buffer at which the video frame was
	  * completed.
	  *
	  * Set iff stopReason == VIDEO_FRAME_PRODUCED.
	  */
	std::ptrdiff_t videoFrameSampleOffset;
};

}

#endif
