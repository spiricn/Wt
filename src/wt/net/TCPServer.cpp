#include "wt/stdafx.h"

#include "wt/net/TCPServer.h"
#include "wt/Exception.h"

#define TD_TRACE_TAG "TCPServer"

namespace wt{

namespace net{

#if defined(WIN32)

TCPServer::TCPServer(){
}

TCPServer::~TCPServer(){
}

void TCPServer::init(const std::string& address, unsigned short port){
	mSocket.init(address, port);
	
	if( listen(mSocket.getWin32SockHandle(), 1) == SOCKET_ERROR ){
		WT_THROW("%s", Socket::getErrorString().c_str());
	}
}

TCPServer::SocketPtr TCPServer::acceptClient(){
	int length = sizeof(SOCKADDR);

	SOCKADDR_IN addr;
	SOCKET s = accept(mSocket.getWin32SockHandle(),
		(SOCKADDR*)&addr, &length);

	if(s != INVALID_SOCKET){
		return new Socket(s, addr);
	}
	else{
		WT_THROW("%s",  Socket::getErrorString().c_str());
	}
}

#else

	#error Not implemented on this platform

#endif


}; // </net>

}; // </wt>