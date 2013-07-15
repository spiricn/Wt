#include "wt/stdafx.h"

#include "wt/AIOStream.h"


namespace wt{

void AIOStream::setMode(Mode mode){
	mMode = mode;
}

AIOStream::AIOStream() : mMode(eMODE_NONE){
}

AIOStream::~AIOStream(){
}

AIOStream::Mode AIOStream::getMode() const{
	return mMode;
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
	std::getline(*this, dst, '\0');
	return dst.size();
}

int64_t AIOStream::seek(int64_t position){
	return seek(eSEEK_BEGGINING, position);
}

}; // </wt>


namespace std{
	void getline(wt::AIOStream& stream, std::string& dst, char delim){
		while(1){
			int16_t c = stream.get();
			if(c < 0){
				// Error
				break;
			}
			

			if((char)c == delim){
				break;
			}
			else{
				dst.push_back((char)c);
			}
		}
	}
}; // </std>