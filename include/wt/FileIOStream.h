#pragma once
#ifndef WT_FILEIOSTREAM_H
#define WT_FILEIOSTREAM_H

#include "wt/AIOStream.h"

namespace wt{

class FileIOStream : public AIOStream{
public:
	FileIOStream(const String& path, Mode mode);

	FileIOStream(std::ostream& stdstream, Mode mode);

	FileIOStream(FILE* fd, Mode mode);

	FileIOStream();

	~FileIOStream();

	void open(const String& path, Mode mode);

	void close();

	int64_t read(void* dst, int64_t size);

	int64_t write(const void* src, int64_t size);

	int64_t seek(SeekOrigin origin, int64_t offset);

	int64_t tell();

	int64_t getSize();

	bool isOpen();

	bool isReadable();

	bool isWritable();

	bool isSeekable();

	void flush();
private:
	FILE* mFile;
	std::ostream* mStream;

	bool mFileOwned;
}; // </FileInputStream>

}; // </wt>

#endif // </WT_FILEIOSTREAM_H>