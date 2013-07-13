#include "wt/stdafx.h"

#include "wt/FileIOStream.h"

namespace wt{

FileIOStream::FileIOStream(const String& path, Mode mode) : mFile(NULL){
	open(path, mode);
}

FileIOStream::FileIOStream() : mFile(NULL){
}

FileIOStream::~FileIOStream(){
	if(isOpen()){
		close();
	}
}

void FileIOStream::open(const String& path, Mode mode){
	close();

	mFile = fopen(path.c_str(), mode == eMODE_READ ? "rb" : "wb");
	setMode(mode);
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

	return fread(dst, 1, size, mFile);
}

int64_t FileIOStream::write(const void* src, int64_t size){
	if(!isWritable()){
		return -1;
	}

	return fwrite(src, 1, size, mFile);
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