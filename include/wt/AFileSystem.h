#pragma once
#ifndef WT_AFILESYSTEM_H
#define WT_AFILESYSTEM_H

#include "wt/Sp.h"
#include "wt/AIOStream.h"

namespace wt{

typedef Sp<AIOStream> StreamPtr;

class AFileSystem{
public:
	virtual ~AFileSystem(){
	}

	virtual StreamPtr open(const String& uri, AIOStream::Mode mode) = 0;
}; // </AFileSystem>

}; // </wt>

#endif // </WT_AFILESYSTEM_H>