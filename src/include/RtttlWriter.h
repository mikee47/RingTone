/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * RtttlWriter.h - Support for reading RTTTL files
 *
 * RTTTL conversion code based on https://github.com/end2endzone/NonBlockingRTTTL
 *
 * @author Sept 2019 mikee47 <mike@sillyhouse.net>
 *
 ****/

#pragma once

#include "RtttlParser.h"
#include <Print.h>

namespace RingTone
{
class RtttlWriter
{
public:
	RtttlWriter(Print& out) : out(out)
	{
	}

	void beginTune(const RtttlHeader& header);
	void addNote(const NoteDef& note);
	void endTune();
	void addComment(const String& text);

	void addTune(RtttlParser& parser);
	void addAllTunes(RtttlParser& parser);

private:
	Print& out;
	RtttlHeader header;
	unsigned tuneCount = 0;
	unsigned noteCount = 0;
};

}; // namespace RingTone
