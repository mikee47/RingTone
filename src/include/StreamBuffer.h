/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * StreamBuffer.h - Buffering for more efficient reading of byte streams
 *
 * @author Sept 2019 mikee47 <mike@sillyhouse.net>
 *
 ****/

#pragma once

#include <Data/Stream/DataSourceStream.h>
#include <memory>

template <unsigned bufSize> class StreamBuffer
{
public:
	void setStream(IDataSourceStream* stream)
	{
		this->stream.reset(stream);
		if(stream == nullptr) {
			streamPos = 0;
			pos = len = 0;
		} else {
			streamPos = stream->seekFrom(0, SeekOrigin::Current);
			fill();
		}
	}

	bool setPos(unsigned pos)
	{
		if(!stream) {
			return false;
		}

		if(stream->seekFrom(pos, SeekOrigin::Start) < 0) {
			return false;
		}

		streamPos = pos;
		fill();
		return !eof();
	}

	char peekChar()
	{
		return (pos < len) ? data[pos] : '\0';
	};

	char readChar()
	{
		char c = data[pos++];
		if(pos >= len) {
			fill();
		}
		return c;
	};

	unsigned getPos()
	{
		return streamPos - len + pos;
	}

	bool eof()
	{
		return len == 0;
	}

private:
	void fill()
	{
		len = stream->readMemoryBlock(data, sizeof(data));
		stream->seek(len);
		streamPos += len;
		pos = 0;
	};

private:
	std::unique_ptr<IDataSourceStream> stream;
	unsigned streamPos = 0;
	char data[bufSize];
	uint8_t pos = 0;
	uint8_t len = 0;
};
