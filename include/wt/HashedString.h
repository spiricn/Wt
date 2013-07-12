#ifndef WT_HASHEDSTRING_H
#define WT_HASHEDSTRING_H


#include "wt/stdafx.h"


#include "wt/Log.h"
#include "wt/Exception.h"

namespace wt{

class HashedString{
private:
	String mString;
	Uint32 mHash;

	Uint32 hash(const String& x, int M=200);


public:
	HashedString();

	HashedString(const std::string& str);

	HashedString(const char* str);

	const String& getString() const;

	const Uint32 getHashCode() const;

	HashedString& operator=(const std::string& str);

	HashedString& operator=(const char* str);

	HashedString& operator=(const HashedString& other);

	bool operator==(const HashedString& other) const;

	const char* c_str() const;

	bool operator!=(const HashedString& other) const;

	bool operator<(const HashedString& other) const;
};

inline size_t hash_value(const HashedString& str);

}; // </wt>

#endif
