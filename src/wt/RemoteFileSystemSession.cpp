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
			WT_ASSERT(mStream->isOpen(), "Stream not open? %s", uri.c_str());
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
			delete[] bfr;

			try{
				mSocket->sendPacket(p);
			}catch(...){
				goto disconnected;
			}
		}
		else if(packetId == rfsp::eCODE_SEEK){
			int8_t origin = p.read<int8_t>();

			int64_t pos = p.read<int64_t>();

			int64_t res = mStream->seek(static_cast<AIOStream::SeekOrigin>(origin), pos);

			p.clear();
			p.write(res);

			try{
				mSocket->sendPacket(p);
			}catch(...){
				goto disconnected;
			}
		}
		else if(packetId == rfsp::eCODE_TELL){
			int64_t pos = mStream->tell();

			p.clear();
			p.write(pos);

			try{
				mSocket->sendPacket(p);
			}catch(...){
				goto disconnected;
			}
		}
		else if(packetId == rfsp::eCODE_IS_OPEN){
			p.clear();
			int8_t res = mStream->isOpen() ? 1 : 0;
			p.write(res);

			try{
				mSocket->sendPacket(p);
			}catch(...){
				goto disconnected;
			}
		}
		else if(packetId == rfsp::eCODE_GET_SIZE){
			int64_t size = mStream->getSize();

			p.clear();
			p.write(size);

			try{
				mSocket->sendPacket(p);
			}catch(...){
				goto disconnected;
			}
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
