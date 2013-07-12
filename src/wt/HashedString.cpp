#include "wt/stdafx.h"

#include "wt/HashedString.h"

#define TD_TRACE_TAG "HashedString"

namespace wt{

uint32_t HashedString::hash(const String& x, int M) {
#if 0
	uint32_t i, sum;
	for(sum=0, i=0; i < x.length(); i++)
		sum += x[i];
	return sum % M;
#else
    unsigned hash = M;
	const char* s = x.c_str();
    while (*s)
    {
        hash = hash * 101  +  *s++;
    }
    return hash;
#endif
}

HashedString::HashedString() : mHash(0), mString(""){
}

HashedString::HashedString(const std::string& str){
	*this = str;
}

HashedString::HashedString(const char* str){
	*this = str;
}

const String& HashedString::getString() const{
	return mString;
}

const uint32_t HashedString::getHashCode() const{
	return mHash;
}

HashedString& HashedString::operator=(const std::string& str){
	mString = str; mHash = hash(mString);
	return *this;
}

HashedString& HashedString::operator=(const char* str){
	mString = str; mHash = hash(mString);
	return *this;
}

HashedString& HashedString::operator=(const HashedString& other){
	mString = other.mString; mHash = other.mHash;
	return *this;
}

bool HashedString::operator==(const HashedString& other) const{
	#ifdef _DEBUG
		if(getHashCode() == other.getHashCode() && getString().compare(other.getString())!=0){
			WT_THROW("Hashed string collision \"%s\"==\"%s\" && %d!=%d", mString.c_str(),
				other.mString.c_str(), mHash, other.mHash);
		}
	#endif

	return mHash == other.mHash;
}

const char* HashedString::c_str() const{
	return mString.c_str();
}

bool HashedString::operator!=(const HashedString& other) const{
	return !(*this==other);
}

bool HashedString::operator<(const HashedString& other) const{
	return mHash < other.mHash;
}

size_t hash_value(const HashedString& str){
	return str.getHashCode();
}

}; // </wt>