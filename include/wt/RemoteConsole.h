#ifndef WT_REMOTECONSOLE_H
#define WT_REMOTECONSOLE_H


#include "wt/Thread.h"
#include "wt/net/TCPServer.h"
#include "wt/EventManager.h"

namespace wt{

class RemoteConsoleCmdEvent : public Event{
protected:
	void serialize(LuaObject& dst){
		dst.Set("command", command.c_str());
	}

	void deserialize(LuaObject& src){
	}

public:
	static const EvtType TYPE;

	String command;

	RemoteConsoleCmdEvent(const String& command) : command(command){
	}

	const EvtType& getType() const{
		return TYPE;
	}

}; // </RemoteConsoleCmdEvent>

const EvtType RemoteConsoleCmdEvent::TYPE = "RemoteConsoleCmd";


class RemoteConsole : public Thread{
private:
	net::TCPServer mServer;

	static const char* TAG;

public:
	RemoteConsole(){
		EventManager::getSingleton().registerInternalEvent(RemoteConsoleCmdEvent::TYPE);
	}

	void run(){
		net::TCPServer::SocketPtr client;

		try{
			LOGD(TAG, "Accepting remote client...");
			mServer.init("127.0.0.1", 13099);
			client = mServer.acceptClient();
		}catch(Exception& e){
			return;
		}

		LOGI(TAG, "Client accepted, receiving...");

		net::Packet packet;

		while(true){
			try{
				client->recvPacket(packet);
			}catch(Exception& e){
				break;
			}

			EventManager::getSingleton().queueEvent(
				new RemoteConsoleCmdEvent((const char*)packet.getPayload())
				);
		}
	}
};
const char* RemoteConsole::TAG = "RemoteConsole";

}; // </wt>

#endif // </WT_REMOTECONSOLE_H>