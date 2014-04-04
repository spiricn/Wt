#include "wt/stdafx.h"

#include "wt/HashedString.h"

#define TD_TRACE_TAG "HashedString"

namespace wt
{

HashedString::HashValue calcHash(const String& x, int m=200);

HashedString::HashedString() : mValue(0), mString(""){
	*this = String("");
}

HashedString::HashedString(const char* str){
	*this = str;
}

HashedString::HashedString(const std::string& str){
	*this = str;
}

const String& HashedString::getString() const{
	return mString;
}

const HashedString::HashValue HashedString::getValue() const{
	return mValue;
}

HashedString& HashedString::operator=(const String& str){
	return operator=(str.c_str());
}

HashedString& HashedString::operator=(const char* str){
	mString = str;
	mValue = calcHash(mString);
	return *this;
}

HashedString& HashedString::operator=(const HashedString& other){
	mString = other.mString;
	mValue = other.mValue;

	return *this;
}

bool HashedString::operator==(const HashedString& other) const{
	#ifdef _DEBUG
	if(mValue == other.mValue && mString.compare(other.mString) !=0){
		WT_THROW("Hashed string collision \"%s\"==\"%s\" && %d!=%d", mString.c_str(),
			other.mString.c_str(), mValue, other.mValue);
	}
	#endif

	return mValue == other.mValue;
}

const char* HashedString::c_str() const{
	return mString.c_str();
}

bool HashedString::operator!=(const HashedString& other) const{
	return !(*this==other);
}

bool HashedString::operator<(const HashedString& other) const{
	return mValue < other.mValue;
}

HashedString::HashValue calcHash(const String& x, int m) {
    HashedString::HashValue hash = m;

	const char* s = x.c_str();

    while (*s){
        hash = hash * 101  +  *s++;
    }

    return hash;
}

} // </wt>