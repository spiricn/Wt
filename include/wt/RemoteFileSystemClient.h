#ifndef WT_REMOTEFILESYSTEMCLIENT_H
#define WT_REMOTEFILESYSTEMCLIENT_H

#include "wt/stdafx.h"
#include "wt/AFileSystem.h"
#include "wt/net/Socket.h"

namespace wt
{

class RemoteFileSystemClient : public AFileSystem{
public:
	RemoteFileSystemClient();

	StreamPtr open(const String& uri, AIOStream::Mode mode);

private:
	net::Socket mSocket;

}; // </RemoteFileSystemClient>

} // </wt>

#endif // </WT_REMOTEFILESYSTEMCLIENT_H>
