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