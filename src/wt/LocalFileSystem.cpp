#include "wt/stdafx.h"

#include "wt/LocalFileSystem.h"

#include "wt/Utils.h"

#define TD_TRACE_TAG "LocalFileSystem"

#define PATH_SEPARATOR "/"

namespace wt
{

LocalFileSystem::LocalFileSystem(const String& root) : mRoot(""){
	String uri = root;
	uri = utils::replacePathSplitters(uri, PATH_SEPARATOR[0]);

	if(uri[ uri.size()-1 ] == PATH_SEPARATOR[0]){
		uri = uri.substr(0, uri.size() - 1);

		TRACEW("Local file system root uri ending with trailing slash, changed to \"%s\" from \"%s\"", 
			uri.c_str(), root.c_str());
	}

	mRoot = uri;
}

String LocalFileSystem::getRoot() const{
	return mRoot;
}

Sp<AIOStream> LocalFileSystem::open(const String& uri, AIOStream::Mode mode){
	String relUri = utils::toRelative(mRoot, uri);
	return new FileIOStream(/* convert to absolute uri */ mRoot + PATH_SEPARATOR +  relUri, mode);
}

} // </wt>
