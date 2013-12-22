#include "wt/stdafx.h"

#include "wt/AIOStream.h"


namespace wt{

AIOStream::AIOStream(){
}

AIOStream::~AIOStream(){
}

int16_t AIOStream::get(){
	char res;
	return read(&res, 1) == 1 ? (int16_t)res : -1;
}

int64_t AIOStream::print(const char* fmt, ...){
	va_list args;
	va_start(args, fmt);

	// TODO dynamic buffer size?
	char bfr[1024];
	memset(bfr, 0x00, 1024);
	vsprintf(bfr, fmt, args);

	va_end(args);


	return write(bfr, strlen(bfr));
}

int64_t AIOStream::put(char c){
	return write(&c, 1);
}

int64_t AIOStream::puts(const String& src){
	return write(src.c_str(), src.size()+1);
}

int64_t AIOStream::gets(String& dst){
	return getLine(dst, '\0');
}

int64_t AIOStream::seek(int64_t position){
	return seek(eSEEK_BEGGINING, position);
}

bool AIOStream::isReadable(){
	return isOpen() && getMode() == eMODE_READ;
}

bool AIOStream::isWritable(){
	return isOpen() && getMode() == eMODE_WRITE;
}

void AIOStream::flush(){
}

int64_t AIOStream::getLine(String& dst, char delim, uint32_t limit){
	char c = 0;
	uint32_t numRead = 0;

	do{
		int16_t rc = get();
		if(c < 0){
			// Error
			return -1;
		}
		else{
			++numRead;
			c = (char)rc;

			if(c != delim){
				dst.push_back(c);
			}
		}
	}while(c != delim && numRead < limit);

	return numRead;
}

}; // </wt>