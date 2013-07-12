#include "wt/stdafx.h"

#include "wt/net/Socket.h"
#include "wt/Exception.h"
#include "wt/Timer.h"

#define TD_TRACE_TAG "Socket"

namespace wt{

namespace net{

#if defined(WIN32)

#pragma comment(lib, "wsock32.lib")

static WSAData gWSAData;

void Socket::initializeLib(){
	if(WSAStartup( MAKEWORD( 2, 1 ), &gWSAData) != 0){
		WT_EXCEPT(TD_TRACE_TAG,
			"Error initializing socket system");
	}
}

bool dnsLookup(const String& addr, IN_ADDR* res){
	hostent* host = gethostbyname(addr.c_str());

	if(host==NULL){
		return false;
	}

	*res  = *((struct in_addr*)host->h_addr);

	return true;
}

bool initAddress(SOCKADDR_IN& saddr, const std::string& addr, unsigned short port, bool getByName){
	memset(&saddr, 0x00, sizeof(SOCKADDR_IN));

	saddr.sin_family = AF_INET;

	if(addr.size()==0){
		saddr.sin_addr.S_un.S_addr = INADDR_ANY;
	}
	else{
		if(getByName){
			if(dnsLookup(addr, &saddr.sin_addr)==false)
				return false;
		}
		else{
			saddr.sin_addr.S_un.S_addr = inet_addr(addr.c_str());
		}
	}

	saddr.sin_port = htons(port);

	return true;
}


Socket::Socket() : mSocket(INVALID_SOCKET){
}

Socket::Socket(SOCKET sock, SOCKADDR_IN addr) : mSocket(sock), mAddr(addr){
}

String Socket::getErrorString(){
	int code = WSAGetLastError();
	TCHAR* errString = NULL;

	int size = FormatMessage(	FORMAT_MESSAGE_ALLOCATE_BUFFER |
							FORMAT_MESSAGE_FROM_SYSTEM,
								0,
								code,
								0,
								(LPTSTR)&errString,
								0,
								0 );

	DWORD c = GetLastError();

	String s;
	TCHAR codeStr[512];
	wsprintf(codeStr, TEXT("[%d]"), code);
	s.append(codeStr);


	TCHAR b[2] = {' ', 0};

	LPTSTR p = errString ;
	while(*p){
		b[0] = (TCHAR)*p;
		s.append(b);
		p++;
	}

	LocalFree( errString );

	return s;
}


String Socket::getAddressString() const{
	return inet_ntoa(mAddr.sin_addr);
}

void Socket::close(){
	if(mSocket != INVALID_SOCKET){
		if(closesocket(mSocket) != SOCKET_ERROR){
			mSocket = INVALID_SOCKET;
		}
		else{
			//throw SocketException(getErrorString());
		}
	}
}

Socket::~Socket(){
	close();
}

bool Socket::connectToServer(const String& address, Uint16 port, float timeout){
	init();

	Timer time;

	SOCKADDR_IN serverAddr;
	if(initAddress(serverAddr, address, port, true) == false){
		WT_EXCEPT(TD_TRACE_TAG, getErrorString().c_str());
	}

	int res = SOCKET_ERROR;

	time.reset();
	while(timeout < 0.0f || time.getSeconds() < timeout){
		res = connect(mSocket, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR));

		if(res != SOCKET_ERROR){
			mServerAddr = serverAddr;
			return true;
			break;
		}
	}

	return false;
}

void Socket::init(const std::string& address, unsigned short port)
{
	close();

	if(initAddress(mAddr, address, port, true) == false){
		WT_EXCEPT(TD_TRACE_TAG,
			"%s", getErrorString().c_str());
	}

	mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(mSocket == SOCKET_ERROR){
		WT_EXCEPT(TD_TRACE_TAG,
			"%s", getErrorString().c_str());
	}

	if( bind(mSocket, (SOCKADDR*)&mAddr, sizeof(SOCKADDR)) == SOCKET_ERROR ){
		WT_EXCEPT(TD_TRACE_TAG,
			"%s", getErrorString().c_str());
	}
}



void Socket::sendBfr(const void* bfr, int size){
	int bytesWritten=0;
	int s=0;

	while(bytesWritten < size){
		s = send(mSocket, (const char*)bfr+bytesWritten, size-bytesWritten, 0);

		if(s<=0){
			WT_EXCEPT(TD_TRACE_TAG,
			"Error sending buffer \"%s\"", getErrorString().c_str()
			);
		}

		bytesWritten += s;
	}
}

void Socket::recvBfr(void* bfr, int size){
	int bytesRead=0;
	int r=0;

	while(bytesRead < size){
		r = recv(mSocket, (char*)bfr+bytesRead, size-bytesRead, 0);

		if(r<=0){
			WT_EXCEPT(TD_TRACE_TAG,
			"Error recieving buffer \"%s\"", getErrorString().c_str()
			);
		}

		bytesRead += r;
	}
}

void Socket::sendPacket(const net::Packet& packet){
	Uint32 payloadSize = packet.getPayloadSize();
	Int32 id = packet.getId();

	// send payload size
	sendBfr(&payloadSize, sizeof(Uint32));

	// send packet ID
	sendBfr(&id, sizeof(Uint32));

	// send the actual data
	sendBfr(packet.getPayload(), packet.getPayloadSize());
}

void Socket::recvPacket(net::Packet& packet){
	packet.clear();

	Uint32 payloadSize;
	Int32 id;

	// recieve payload size
	recvBfr(&payloadSize, sizeof(Uint32));

	// recieve packet ID
	recvBfr(&id, sizeof(Int32));

	// initialize payload memory
	packet = net::Packet(id, NULL, payloadSize);

	recvBfr(packet.getPayload(), payloadSize);
}

#else

	#error Not implemented on this platform

#endif

}; // </net>

}; // </wt>
