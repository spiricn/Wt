#ifndef WT_SOCKET_H
#define WT_SOCKET_H

#include "wt/stdafx.h"

#include "wt/net/Packet.h"

namespace wt{

namespace net{

class Socket{
private:
	static const char* TAG;

public:
	static void initializeLib();

	static String getErrorString();

	Socket();

	~Socket();

	String getAddressString() const;

	void close();

	void init(const std::string& address="", Uint16 port=0);

	void sendBfr(const ByteBuffer& bfr);

	void sendBfr(const void* bfr, int size);

	void recvBfr(void* bfr, int size);

	void sendPacket(const net::Packet& packet);

	void recvPacket(net::Packet& packet);

	bool connectToServer(const String& addr, Uint16 port, float timeout=1.0f);

#if defined(WIN32)
private:
	SOCKET mSocket;
	SOCKADDR_IN mAddr;
	SOCKADDR_IN mServerAddr;

public:
	SOCKET getWin32SockHandle() const{
		return mSocket;
	}

	Socket(SOCKET sock, SOCKADDR_IN addr);
#endif // </WIN32>

}; // Socket

}; // </net>

}; // </wt>

#endif // </WT_SOCKET_H>