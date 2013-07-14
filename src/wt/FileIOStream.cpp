#include "wt/stdafx.h"

#include "wt/FileIOStream.h"

#define TD_TRACE_TAG "FileIOStream"

namespace wt{


// TODO handle not owned file streams (fd and std::ostream)

FileIOStream::FileIOStream(const String& path, Mode mode) : mFile(NULL), mFileOwned(true), mStream(NULL){
	open(path, mode);
}

FileIOStream::FileIOStream() : mFile(NULL), mFileOwned(true), mStream(NULL){
}

FileIOStream::FileIOStream(std::ostream& stdstream, Mode mode) : mFile(NULL), mFileOwned(false), mStream(&stdstream){
}

FileIOStream::FileIOStream(FILE* fd, Mode mode) : mFile(fd), mFileOwned(false), mStream(NULL){
}

FileIOStream::~FileIOStream(){
	if(isOpen()){
		close();
	}
}

void FileIOStream::open(const String& path, Mode mode){
	close();

	mFile = fopen(path.c_str(), mode == eMODE_READ ? "rb" : "wb");

	if(!mFile){
		TRACEE("fopen failed for \"%s\" (mode=\"%s\" errno=%d, desc=\"%s\")",
			path.c_str(), mode == eMODE_READ ? "rb" : "wb", errno, strerror(errno));
	}
	
	setMode(mFile ? mode : eMODE_NONE);
}

void FileIOStream::close(){
	if(isOpen()){
		fclose(mFile);
		mFile = NULL;
	}
}

int64_t FileIOStream::read(void* dst, int64_t size){
	if(!isReadable()){
		return -1;
	}

	size_t read = fread(dst, 1, size, mFile);
	if(read != size){
		LOGE("fread failed (read %d / %d)", read, size);
	}

	return read;
}

int64_t FileIOStream::write(const void* src, int64_t size){
	if(!isWritable()){
		return -1;
	}

	size_t written = fwrite(src, 1, size, mFile);

	if(written != size){
		TRACEE("fwrite failed (wrote %d / %d)", written, size);
	}

	return written;
}

int64_t FileIOStream::seek(SeekOrigin origin, int64_t offset){
	if(!isOpen()){
		return -1;
	}

	int rc = fseek(mFile, offset, origin == eSEEK_BEGGINING ? SEEK_SET : origin == eSEEK_CURRENT ? SEEK_CUR : SEEK_END);
	if(rc){
		// Seek failed
		return -1;
	}

	return tell();
}

int64_t FileIOStream::tell(){
	if(!isOpen()){
		return -1;
	}

	return ftell(mFile);
}

int64_t FileIOStream::getSize(){
	if(!isOpen()){
		return -1;
	}

	int64_t pos = tell();

	int64_t size  = seek(eSEEK_END, 0);

	AIOStream::seek(pos);

	return size;
}

bool FileIOStream::isOpen(){
	return mFile != NULL;
}

bool FileIOStream::isReadable(){
	return isOpen() && getMode() == eMODE_READ;
}

bool FileIOStream::isWritable(){
	return isOpen() && getMode() == eMODE_WRITE;
}

bool FileIOStream::isSeekable(){
	return true;
}

void FileIOStream::flush(){
	if(isOpen()){
		fflush(mFile);
	}
}


}; // </wt>