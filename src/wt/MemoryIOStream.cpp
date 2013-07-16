#include "wt/stdafx.h"

#include "wt/MemoryIOStream.h"

namespace wt{

MemoryIOStream::MemoryIOStream(void* source, uint64_t size) : mSource((uint8_t*)source), mPtr(0), mSize(size){
}

int64_t MemoryIOStream::read(void* dst, int64_t size){
	// TODO checks
	memcpy(dst, mSource + mPtr, size);
	mPtr += size;

	return size;
}

int64_t MemoryIOStream::write(const void* src, int64_t size){
	return -1;
}

int64_t MemoryIOStream::seek(SeekOrigin origin, int64_t offset){
	return -1;
}
	
int64_t MemoryIOStream::tell(){
	return isOpen() ? mPtr : -1;
}

int64_t MemoryIOStream::getSize(){
	return mSize;
}

bool MemoryIOStream::isOpen(){
	return mSource != NULL;
}

bool MemoryIOStream::isSeekable(){
	return false;
}

}; // </wt>
