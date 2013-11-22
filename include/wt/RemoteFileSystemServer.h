#ifndef WT_REMOTEFILESYSTEMSERVER_H
#define WT_REMOTEFILESYSTEMSERVER_H

#include "wt/stdafx.h"
#include "wt/Thread.h"
#include "wt/net/TCPServer.h"

namespace wt
{

class RemoteFileSystemServer : public Thread{
public:
	RemoteFileSystemServer();

	void startServer(const String& root, uint32_t port);

	void run();

private:
	net::TCPServer mServer;
	String mRoot;

}; // </RemoteFileSystemServer>

} // </wt>

#endif // </WT_REMOTEFILESYSTEMSERVER_H>
