#pragma once
#ifndef WT_LOCALFILESYSTEM_H
#define WT_LOCALFILESYSTEM_H

#include "wt/AFileSystem.h"
#include "wt/FileIOStream.h"

namespace wt{

class LocalFileSystem : public AFileSystem{
public:
	LocalFileSystem(const String& root);

	Sp<AIOStream> open(const String& uri, AIOStream::Mode mode);

	String getRoot() const;

private:
	String mRoot;
}; // </LocalFileSystem>

} // <wt>

#endif // </WT_LOCALFILESYSTEM_H>