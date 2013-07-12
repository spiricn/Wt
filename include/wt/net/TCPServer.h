#ifndef WT_TCPSERVER_H
#define WT_TCPSERVER_H

#include "wt/net/Socket.h"
#include "wt/Sp.h"

namespace wt{

namespace net{

class TCPServer{
private:
	static const char* TAG;

	Socket mSocket;

public:

	typedef Sp<Socket> SocketPtr;
	
public:
	TCPServer();

	~TCPServer();

	void init(const String& address="", unsigned short port=0);

	SocketPtr acceptClient();	

}; // </Server>

}; // </net>

}; // </wt>

#endif // </WT_TCPSERVER_H>