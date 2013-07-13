#include "wt/stdafx.h"

#include "wt/LocalFileSystem.h"

namespace wt{

LocalFileSystem::LocalFileSystem(const String& root) : mRoot(root){
}

Sp<AIOStream> LocalFileSystem::open(const String& uri, AIOStream::Mode mode){
	return new FileIOStream(mRoot + uri, mode);
}

}; // </wt>