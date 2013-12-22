#include "wt/stdafx.h"

#include "wt/ZipInputStream.h"

namespace wt{

ZipInputStream::ZipInputStream(PHYSFS_file* file) : mFile(file), mMode(eMODE_INVALID){
	mMode = eMODE_READ;
}

ZipInputStream::~ZipInputStream(){
	if(mFile){
		PHYSFS_close(mFile);
		mFile = NULL;
	}
}

AIOStream::Mode ZipInputStream::getMode() const{
	return mMode;
}

int64_t ZipInputStream::read(void* dst, int64_t size){
	if(!isOpen()){
		return -1;
	}

	return PHYSFS_read(mFile, dst, 1, size);
}

int64_t ZipInputStream::write(const void* src, int64_t size){
	return -1;
}

int64_t ZipInputStream::seek(SeekOrigin origin, int64_t offset){
	if(!isOpen()){
		return -1;
	}

	if(origin == AIOStream::eSEEK_BEGGINING){
		PHYSFS_seek(mFile, offset);
	}
	else if(origin == AIOStream::eSEEK_CURRENT){
		PHYSFS_seek(mFile, tell() + offset);
	}
	else if(origin == AIOStream::eSEEK_END){
		PHYSFS_seek(mFile, getSize() + offset);
	}

	return tell();
}

int64_t ZipInputStream::tell(){
	if(!isOpen()){
		return -1;
	}

	return PHYSFS_tell(mFile);
}

int64_t ZipInputStream::getSize(){
	if(!isOpen()){
		return -1;
	}

	return PHYSFS_fileLength(mFile);
}

bool ZipInputStream::isOpen(){
	return mFile != NULL;
}

bool ZipInputStream::isReadable(){
	return isOpen();
}

bool ZipInputStream::isWritable(){
	return false;
}

bool ZipInputStream::isSeekable(){
	if(!isOpen()){
		return false;
	}
	else{
		return true;
	}
}

void ZipInputStream::flush(){
}


}; // </wt>