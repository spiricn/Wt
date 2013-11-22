#include "wt/stdafx.h"

#include "wt/RemoteFileSystemServer.h"
#include "wt/RemoteFileSystemSession.h"

#define TD_TRACE_TAG "RemoteFileSystemServer"

namespace wt
{

RemoteFileSystemServer::RemoteFileSystemServer(){
}

void RemoteFileSystemServer::startServer(const String& root, uint32_t port){
	mServer.init("127.0.0.1", port);

	mRoot = root;

	LOGI("Server started @ %d", port);

	start();
}

void RemoteFileSystemServer::run(){
	LOGI("Server thread running...");

	while(true){
		net::TCPServer::SocketPtr socket = mServer.acceptClient();
		LOGD("Client connected");

		RemoteFileSystemSession* session = new RemoteFileSystemSession(this, socket, mRoot);
		session->start();
	}
}

} // </wt>