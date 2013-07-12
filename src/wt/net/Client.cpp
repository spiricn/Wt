#include "Rc/shared/Client.h"
#include "Rc/shared/PyRc.h"
#include "Rc/shared/Exception.h"

namespace Rc
{

void Client::connectToServer(const std::string& address, unsigned short port)
{
	if(Rc::InitAddr(mServerAddr, address, port)==false){
		Rc_EXCEPT("%s", getErrorString().c_str());
	}

	int res = SOCKET_ERROR;

	BEG_ALLOW_PYTHREADS
		res = connect(mSocket, (SOCKADDR*)&mServerAddr, sizeof(SOCKADDR));
	END_ALLOW_PYTHREADS

	if(res == SOCKET_ERROR ){
		Rc_EXCEPT("%s", getErrorString().c_str());
	}
}

}; // </Rc>