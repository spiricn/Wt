#pragma once
#ifndef WT_ZIPFILESYSTEM_H
#define WT_ZIPFILESYSTEM_H

#include "wt/AFileSystem.h"
#include "wt/ZipInputStream.h"

namespace wt{

/**
 * @todo abstract this class (e.g. AZipFileSystem) so that we're not limited to phyfs
 * Might be a good idea to use something like Minzip http://www.winimage.com/zLibDll/minizip.html
 */
class ZipFileSystem : public AFileSystem{
public:
	ZipFileSystem(const String& filePath, const String& root);

	~ZipFileSystem();

	Sp<AIOStream> open(const String& uri, AIOStream::Mode mode);

	String getRoot() const;
	
private:
	String mRoot;

}; // </ZipFileSystem>

}; // </wt>

#endif // </WT_ZIPFILESYSTEM_H>