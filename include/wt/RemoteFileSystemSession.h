#ifndef WT_REMOTEFILESYSTEMSESSION_H
#define WT_REMOTEFILESYSTEMSESSION_H

#include "wt/stdafx.h"
#include "wt/Thread.h"
#include "wt/net/TCPServer.h"
#include "wt/LocalFileSystem.h"

namespace wt
{

class RemoteFileSystemServer;

class RemoteFileSystemSession : public Thread{
friend class RemoteFileSystemServer;

public:
	void run();

private:

	RemoteFileSystemSession(RemoteFileSystemServer* parent, net::TCPServer::SocketPtr socket, const String& root);

	RemoteFileSystemServer* mParent;
	net::TCPServer::SocketPtr mSocket;
	StreamPtr mStream;
	LocalFileSystem mLocalFileSystem;

}; // </RemoteFileSystemSession>

} // </wt>

#endif // </WT_REMOTEFILESYSTEMSESSION_H>
