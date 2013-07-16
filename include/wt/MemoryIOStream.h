#pragma once
#ifndef WT_MEMORYIOSTREAM_H
#define WT_MEMORYIOSTREAM_H

#include "wt/AIOStream.h"

namespace wt{

class MemoryIOStream : public AIOStream{
public:
	MemoryIOStream(void* source, uint64_t size);

	virtual int64_t read(void* dst, int64_t size);

	virtual int64_t write(const void* src, int64_t size);

	virtual int64_t seek(SeekOrigin origin, int64_t offset);
	
	virtual int64_t tell();

	virtual int64_t getSize();

	virtual bool isOpen();

	virtual bool isSeekable();

private:
	uint8_t* mSource;
	uint64_t mPtr;
	uint64_t mSize;
}; // </MemoryIOStream>

}; // </wt>

#endif // </WT_MEMORYIOSTREAM_H>