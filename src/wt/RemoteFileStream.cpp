#include "wt/stdafx.h"

#include "wt/RemoteFileStream.h"
#include "wt/RemoteFileSystemProtocol.h"

namespace wt
{

int64_t RemoteFileStream::read(void* dst, int64_t size){
	// Send request
	net::Packet packet;

	// Write ID
	int32_t packetId = rfsp::eCODE_READ;
	packet.write(packetId);

	// Write size
	packet.write(size);

	// Send request
	mSocket->sendPacket(packet);

	// Recieve response
	packet.clear();
	mSocket->recvPacket(packet);

	int64_t bytesRead = packet.read<int64_t>();

	packet.read(dst, bytesRead);

	return bytesRead;
}

int64_t RemoteFileStream::write(const void* src, int64_t size){
	WT_THROW("Not implemented");
	return 0;
}

int64_t RemoteFileStream::seek(SeekOrigin origin, int64_t offset){
	WT_THROW("Not implemented");
	return 0;
}
	
int64_t RemoteFileStream::tell(){
	WT_THROW("Not implemented");
	return 0;
}

int64_t RemoteFileStream::getSize(){
	WT_THROW("Not implemented");
	return 0;
}

bool RemoteFileStream::isOpen(){
	WT_THROW("Not implemented");
	return false;
}

bool RemoteFileStream::isSeekable(){
	WT_THROW("Not implemented");
	return false;
}

RemoteFileStream::RemoteFileStream(RemoteFileSystemClient* parent, const String& uri, AIOStream::Mode mode, net::TCPServer::SocketPtr socket) : mParent(parent), mSocket(socket){
	net::Packet packet;

	// Write code
	int32_t code = rfsp::eCODE_STREAM_OPEN;
	packet.write(code);

	// Write URI
	packet.write(uri);

	// Mode
	if(mode == AIOStream::eMODE_READ){
		packet.write(String("r"));
	}
	else if(mode == AIOStream::eMODE_WRITE){
		packet.write(String("w"));
	}
	else{
		WT_THROW("Unsupported open mode %d", mode);
	}

	// Send open request
	mSocket->sendPacket(packet);
}

} // </wt>
