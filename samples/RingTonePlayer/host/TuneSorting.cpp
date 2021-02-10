/*
 * Code used to help merge, tidy and sort tune lists.
 *
 * Also demonstrates use of RtttlWriter.
 */

#include <RttlWriter.h>
#include <Data/Stream/MemoryDataStream.h>
#include <IFS/Host/FileSystem.h>
#include <Data/Stream/IFS/FileStream.h>
#include <HardwareSerial.h>

namespace
{
IFS::Host::FileSystem fileSystem;
}

void dumpTunes()
{
	RingTone::RtttlParser parser;
	auto fs = new IFS::FileStream(&fileSystem);
	fs->open("files/tunes.txt");
	if(parser.begin(fs)) {
		IFS::FileStream fs(&fileSystem);
		fs.open("tunes-out-1.txt", IFS::File::CreateNewAlways | IFS::File::WriteOnly);
		RingTone::RtttlWriter writer(fs);
		writer.addAllTunes(parser);
		Serial.print("Count = ");
		Serial.println(parser.getIndex());
		fs.close(); // Ensure all data is flushed before reading again

		// Dump the generated list again: the two files should be identical

		fs.open("tunes-out-1.txt");
		if(parser.begin(&fs)) {
			IFS::FileStream fs2(&fileSystem);
			fs2.open("tunes-out-2.txt", IFS::File::CreateNewAlways | IFS::File::WriteOnly);
			RingTone::RtttlWriter writer(fs2);
			writer.addAllTunes(parser);
			Serial.print("Count = ");
			Serial.println(parser.getIndex());
		}
	}
}

void sortTunes()
{
	struct Tune {
		RingTone::RtttlHeader header;
		String headerString;
		String content;
		String titles; // Alternate titles found
	};
	Vector<Tune> tunes;

	RingTone::RtttlParser parser;
	auto fs = new IFS::FileStream(&fileSystem);
	fs->open("files/tunes.txt");
	if(!parser.begin(fs)) {
		return;
	}

	do {
		MemoryDataStream mem;
		RingTone::RtttlWriter writer(mem);
		const auto& header = parser.getHeader();
		writer.beginTune(header);

		String headerString;
		mem.moveString(headerString);

		RingTone::NoteDef note;
		while(parser.readNextNote(note)) {
			writer.addNote(note);
		}

		String content;
		mem.moveString(content);

		auto findContent = [&]() -> int {
			for(unsigned i = 0; i < tunes.count(); ++i) {
				auto& tune = tunes[i];
				if(tune.content == content) {
					return i;
				}
			}
			return -1;
		};

		int i = findContent();
		if(i >= 0) {
			auto& tune = tunes[i];
			Serial.printf("'%s' is duplicate of '%s'\r\n", header.title.c_str(), tune.header.title.c_str());
			if(tune.titles) {
				tune.titles += ", ";
			}
			tune.titles += header.title;
			continue;
		}

		auto tune = new Tune({header, headerString, content, nullptr});
		tunes.addElement(tune);
	} while(parser.nextTune());

	tunes.sort([](const Tune& lhs, const Tune& rhs) {
		auto skip = [](const String& s) {
			auto p = s.c_str();
			if(s.startsWith("the ") || s.startsWith("The ")) {
				p += 4;
			} else if(s.startsWith("a ") || s.startsWith("A ")) {
				p += 2;
			}
			return p;
		};
		const char* t1 = skip(lhs.header.title);
		const char* t2 = skip(rhs.header.title);
		int i = strcasecmp(t1, t2);
		if(i == 0) {
			i = lhs.content.length() - rhs.content.length();
		}
		return i;
	});

	IFS::FileStream fs2(&fileSystem);
	fs2.open("tunes-sorted.txt", IFS::File::CreateNewAlways | IFS::File::WriteOnly);
	for(unsigned i = 0; i < tunes.count(); ++i) {
		auto& tune = tunes[i];
		if(tune.titles) {
			fs2.print("# ");
			fs2.println(tune.titles);
		}
		fs2.print(tune.headerString);
		fs2.println(tune.content);
	}
}
