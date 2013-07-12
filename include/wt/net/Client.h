#ifndef _H_CLIENT
#define _H_CLIENT

#include <iostream>

#include "Rc/shared/Utils.h"
#include "Rc/shared/Socket.h"

namespace Rc{

class Client : public Socket{
private:
	SOCKADDR_IN mServerAddr;

public:

	void connectToServer(const std::string& address="", unsigned short port=0);

}; // </Client>


}; // </Rc>

#endif