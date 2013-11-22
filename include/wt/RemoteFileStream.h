#ifndef WT_REMOTEFILESTREAM_H
#define WT_REMOTEFILESTREAM_H

#include "wt/AIOStream.h"
#include "wt/net/TCPServer.h"

namespace wt
{

class RemoteFileSystemServer;
class RemoteFileSystemClient;

class RemoteFileStream : public AIOStream{
friend class RemoteFileSystemClient;

public:
	int64_t read(void* dst, int64_t size);

	int64_t write(const void* src, int64_t size);

	int64_t seek(SeekOrigin origin, int64_t offset);
	
	int64_t tell();

	int64_t getSize();

	bool isOpen();

	bool isSeekable();

private:
	RemoteFileStream(RemoteFileSystemClient* parent, const String& uri, AIOStream::Mode mode, net::TCPServer::SocketPtr socket);

	RemoteFileSystemClient* mParent;
	net::TCPServer::SocketPtr mSocket;

}; // </RemoteFileStream>

} // </wt>

#endif // </WT_REMOTEFILESTREAM_H>
