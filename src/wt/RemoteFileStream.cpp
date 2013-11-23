#include "wt/stdafx.h"

#include "wt/RemoteFileStream.h"
#include "wt/RemoteFileSystemProtocol.h"

namespace wt
{

static net::Packet createPacket(rfsp::OperationCode code){
	net::Packet packet;

	int32_t opCode = static_cast<int32_t>(code);
	packet.write(opCode);

	return packet;
}


int64_t RemoteFileStream::read(void* dst, int64_t size){
	// Send request
	net::Packet packet = createPacket( rfsp::eCODE_READ );

	// Write size
	packet.write(size);

	request(packet);

	int64_t bytesRead = packet.read<int64_t>();

	packet.read(dst, bytesRead);

	return bytesRead;
}

int64_t RemoteFileStream::write(const void* src, int64_t size){
	WT_THROW("Not implemented");
	return 0;
}

int64_t RemoteFileStream::seek(SeekOrigin origin, int64_t offset){
	net::Packet packet = createPacket( rfsp::eCODE_SEEK );

	int8_t _origin = static_cast<int8_t>(origin);

	packet.write(_origin);
	packet.write(offset);

	request(packet);

	return packet.read<int64_t>();
}
	
int64_t RemoteFileStream::tell(){
	net::Packet packet = createPacket( rfsp::eCODE_TELL );

	request(packet);

	return packet.read<int64_t>();
}

int64_t RemoteFileStream::getSize(){
	net::Packet packet = createPacket( rfsp::eCODE_GET_SIZE );

	request(packet);

	return packet.read<int64_t>();
}

void RemoteFileStream::request(net::Packet& pckt){
	mSocket->sendPacket(pckt);

	pckt.clear();
	mSocket->recvPacket(pckt);
}


bool RemoteFileStream::isOpen(){
	net::Packet packet = createPacket( rfsp::eCODE_IS_OPEN );

	request(packet);

	return packet.read<int8_t>() == 0 ? false : true;
}

bool RemoteFileStream::isSeekable(){
	WT_THROW("Not implemented");
	return false;
}

RemoteFileStream::RemoteFileStream(RemoteFileSystemClient* parent, const String& uri, AIOStream::Mode mode, net::TCPServer::SocketPtr socket) : mParent(parent), mSocket(socket){
	net::Packet packet = createPacket( rfsp::eCODE_STREAM_OPEN );

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

	setMode(mode);
}

} // </wt>
