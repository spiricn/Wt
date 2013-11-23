#include "wt/stdafx.h"

#include "wt/ZipFileSystem.h"


namespace wt{

ZipFileSystem::ZipFileSystem(const String& filePath, const String& root) : mRoot(root){
	PHYSFS_init(NULL);

	// TODO handle root directory

	PHYSFS_addToSearchPath(filePath.c_str(), 0);
}

ZipFileSystem::~ZipFileSystem(){
	PHYSFS_deinit();
}


String ZipFileSystem::getRoot() const{
	return mRoot;
}

Sp<AIOStream> ZipFileSystem::open(const String& uri, AIOStream::Mode mode){
	if(!PHYSFS_exists(uri.c_str())){
		LOGW("File \"%s\" does not exist in zip archive", uri.c_str());
	}

	return new ZipInputStream(PHYSFS_openRead(uri.c_str()));
}

}; // </wt>