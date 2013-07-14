#include "wt/stdafx.h"

#include "wt/ZipFileSystem.h"


namespace wt{

ZipFileSystem::ZipFileSystem(const String& root){
	PHYSFS_init(NULL);

	PHYSFS_addToSearchPath(root.c_str(), 0);
}

ZipFileSystem::~ZipFileSystem(){
	PHYSFS_deinit();
}

Sp<AIOStream> ZipFileSystem::open(const String& uri, AIOStream::Mode mode){

	if(!PHYSFS_exists(uri.c_str())){
		LOGW("File \"%s\" does not exist in zip archive", uri.c_str());
	}

	return new ZipInputStream(PHYSFS_openRead(uri.c_str()));
}

}; // </wt>