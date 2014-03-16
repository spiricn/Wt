#include "wt/stdafx.h"

#include "wt/RemoteFileSystemClient.h"
#include "wt/net/TCPServer.h"
#include "wt/RemoteFileStream.h"
#include "wt/Utils.h"


#define PATH_SEPARATOR "/"

namespace wt
{

RemoteFileSystemClient::RemoteFileSystemClient(String root, String address, int32_t port) : mRoot(root), mServerAddress(address), mServerPort(port){
	String uri = root;

	uri = utils::replacePathSplitters(uri, PATH_SEPARATOR[0]);

	if(!uri.empty() && uri[ uri.size()-1 ] == PATH_SEPARATOR[0]){
		uri = uri.substr(0, uri.size() - 1);

		TRACEW("Local file system root uri ending with trailing slash, changed to \"%s\" from \"%s\"", 
			uri.c_str(), root.c_str());
	}

	mRoot = uri;
}

String RemoteFileSystemClient::getRoot() const{
	return mRoot;
}

StreamPtr RemoteFileSystemClient::open(const String& relUri, AIOStream::Mode mode){
	net::TCPServer::SocketPtr socket = new net::Socket;

	socket->connectToServer(mServerAddress, mServerPort, -1.0f);
	
	// TODO memory leak

	return new RemoteFileStream(this, /* convert to absolute uri */ mRoot + PATH_SEPARATOR +  relUri, mode, socket);
}

} // </wt>
