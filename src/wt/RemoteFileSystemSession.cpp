#include "wt/stdafx.h"

#include "wt/RemoteFileSystemSession.h"
#include "wt/RemoteFileSystemProtocol.h"

namespace wt
{
void RemoteFileSystemSession::run(){
	while(true){
		net::Packet p;

		try{
			mSocket->recvPacket(p);
		}catch(...){
			goto disconnected;
		}

		int32_t packetId = p.read<int32_t>();

		if(packetId == rfsp::eCODE_STREAM_OPEN){
			String uri;
			p.read(&uri);

			String modeStr;
			p.read(&modeStr);

			AIOStream::Mode mode = modeStr.compare("r") == 0 ? AIOStream::eMODE_READ : AIOStream::eMODE_WRITE;

			mStream = mLocalFileSystem.open(uri, mode);
			WT_ASSERT(mStream, "Error oppening file %s", uri.c_str());
		}
		else if(packetId == rfsp::eCODE_READ){
			// Requested bytes to be read
			uint64_t bytes = p.read<int64_t>();

			// Allocate buffer
			uint8_t* bfr = new uint8_t[bytes];

			// Read bytes
			int64_t bytesRead = mStream->read(bfr, bytes);

			p.clear();

			// Write header
			p.write(bytesRead);

			// Write data
			p.write(bfr, bytesRead);

			try{
				mSocket->sendPacket(p);
			}catch(...){
				goto disconnected;
			}

			delete[] bfr;
		}
		else{
			WT_THROW("Unsupported code recieved %d", packetId);
		}
	}

disconnected:
	TRACED("Disconnected!");
}

RemoteFileSystemSession::RemoteFileSystemSession(RemoteFileSystemServer* parent, net::TCPServer::SocketPtr socket, const String& root) : mParent(parent), mSocket(socket), mLocalFileSystem(root){
}

} // </wt>
