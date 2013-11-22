#include "wt/stdafx.h"

#include "wt/RemoteFileSystemClient.h"
#include "wt/net/TCPServer.h"
#include "wt/RemoteFileStream.h"

namespace wt
{

RemoteFileSystemClient::RemoteFileSystemClient(){
}

StreamPtr RemoteFileSystemClient::open(const String& uri, AIOStream::Mode mode){
	net::TCPServer::SocketPtr socket = new net::Socket;

	socket->connectToServer("127.0.0.1", 8420, -1.0f);

	return new RemoteFileStream(this, uri, mode, socket);
}

} // </wt>
