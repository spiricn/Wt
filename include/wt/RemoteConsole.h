#ifndef WT_REMOTECONSOLE_H
#define WT_REMOTECONSOLE_H


#include "wt/Thread.h"
#include "wt/net/TCPServer.h"
#include "wt/EventManager.h"

#define TD_TRACE_TAG "RemoteConsole"

namespace wt{

class RemoteConsoleCmdEvent : public AEvent{
protected:
	void serialize(LuaObject& dst){
		dst.Set("command", command.c_str());
	}

	void deserialize(LuaObject& src){
	}

public:
	static const EventType TYPE;

	String command;

	RemoteConsoleCmdEvent(const String& command) : command(command){
	}

	const EventType& getType() const{
		return TYPE;
	}

}; // </RemoteConsoleCmdEvent>

const EventType RemoteConsoleCmdEvent::TYPE = "RemoteConsoleCmd";


class RemoteConsole : public Thread{
private:
	net::TCPServer mServer;

	static const char* TAG;

public:
	RemoteConsole(){
		EventManager::getSingleton().registerEvent(RemoteConsoleCmdEvent::TYPE);
	}

	void run(){
		net::TCPServer::SocketPtr client;

		try{
			LOGD("Accepting remote client...");
			mServer.init("127.0.0.1", 13099);
			client = mServer.acceptClient();
		}catch(Exception& e){
			return;
		}

		LOGI("Client accepted, receiving...");

		net::Packet packet;

		while(true){
			try{
				client->recvPacket(packet);
			}catch(Exception& e){
				break;
			}

			EventManager::getSingleton().emitEvent(
				new RemoteConsoleCmdEvent((const char*)packet.getPayload())
				);
		}
	}
};
const char* RemoteConsole::TAG = "RemoteConsole";

}; // </wt>

#endif // </WT_REMOTECONSOLE_H>