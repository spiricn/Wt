#include "wt/stdafx.h"

#include "wt/LocalFileSystem.h"

#include "wt/Utils.h"

#define PATH_SEPARATOR "/"

namespace wt{

LocalFileSystem::LocalFileSystem(const String& root) : mRoot(""){
	String uri = root;
	utils::replacePathSplitters(uri, PATH_SEPARATOR[0]);

	if(uri[ uri.size()-1 ] == PATH_SEPARATOR[0]){
		uri = uri.substr(0, uri.size() - 1);

		TRACEW("Local file system root uri ending with trailing slash, changed to \"%s\" from \"%s\"", 
			uri.c_str(), root.c_str());
	}

	mRoot = uri;
}

Sp<AIOStream> LocalFileSystem::open(const String& relUri, AIOStream::Mode mode){
	return new FileIOStream(/* convert to absolute uri */ mRoot + PATH_SEPARATOR +  relUri, mode);
}

}; // </wt>