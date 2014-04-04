#ifndef WT_HASHEDSTRING_H
#define WT_HASHEDSTRING_H

#include "wt/stdafx.h"

#include "wt/Exception.h"

namespace wt
{

class HashedString{
public:
	typedef uint32_t HashValue;

public:
	HashedString();

	HashedString(const String& str);

	HashedString(const char* str);

	const String& getString() const;

	const HashValue getValue() const;

	const char* c_str() const;

	HashedString& operator=(const String& str);

	HashedString& operator=(const char* str);

	HashedString& operator=(const HashedString& other);

	bool operator==(const HashedString& other) const;

	bool operator!=(const HashedString& other) const;

	bool operator<(const HashedString& other) const;

private:
	String mString;
	HashValue mValue;

}; // </HashedString>

} // </wt>

#endif  // </WT_HASHEDSTRING_H>
