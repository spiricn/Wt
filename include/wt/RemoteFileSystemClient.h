#ifndef WT_REMOTEFILESYSTEMCLIENT_H
#define WT_REMOTEFILESYSTEMCLIENT_H

#include "wt/stdafx.h"
#include "wt/AFileSystem.h"
#include "wt/net/Socket.h"

namespace wt
{

class RemoteFileSystemClient : public AFileSystem{
public:
	RemoteFileSystemClient(String root, String address, int32_t port);

	StreamPtr open(const String& uri, AIOStream::Mode mode);

	String getRoot() const;

private:
	net::Socket mSocket;
	String mServerAddress;
	uint32_t mServerPort;
	String mRoot;

}; // </RemoteFileSystemClient>

} // </wt>

#endif // </WT_REMOTEFILESYSTEMCLIENT_H>
