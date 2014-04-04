#include "demo/stdafx.h"

#if 0
using namespace wt;

#include "demo/DemoManager.h"

// Demo declarations
WT_DECLARE_DEMO(TestDemo);
WT_DECLARE_DEMO(BasicDemo);
WT_DECLARE_DEMO(FogDemo);
WT_DECLARE_DEMO(PhysicsDemo);
WT_DECLARE_DEMO(GuiDemo);
WT_DECLARE_DEMO(ParticleDemo);
WT_DECLARE_DEMO(ScriptDemo);
WT_DECLARE_DEMO(SoundDemo);

#if defined(WT_DEMO_NO_CONSOLE) && defined(WIN32)
	#pragma comment(linker, "/SUBSYSTEM:windows")
#endif

#ifndef WT_DEMO_NO_CONSOLE
int main(){
#else
#if defined(WIN32)
	int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow){
#else
	#error Not implemented on this platform
#endif
#endif
	DemoManager::getSingleton().initialize();


	// Demo registrations
	{
		WT_REGISTER_DEMO(TestDemo);

		WT_REGISTER_DEMO(BasicDemo);

		WT_REGISTER_DEMO(FogDemo);

		WT_REGISTER_DEMO(PhysicsDemo);

		WT_REGISTER_DEMO(GuiDemo);

		WT_REGISTER_DEMO(ParticleDemo);

		WT_REGISTER_DEMO(ScriptDemo);

		WT_REGISTER_DEMO(SoundDemo);
	}

	DemoManager::getSingleton().start();

	return 0;
}

#else


#include <wt/Event.h>

class IEventEmitter{
public:
};

class IEventListener{
public:
	virtual bool handleEvent(const wt::EvtPtr evt) = 0;
};



class EventManager{
public:
	enum ConnectionType{
		eCONNECTION_DIRECT,
		eCONNECTION_QUEUED,
		eCONNECTION_MAX
	};


private:
	struct EventQueueEntry{
		const wt::EvtPtr event;
		IEventEmitter* emitter;

		EventQueueEntry(const wt::EvtPtr event, IEventEmitter* emitter) : emitter(emitter), event(event){
		}
	};

	typedef std::vector<wt::EvtType> EventTypeList;

	typedef std::queue<EventQueueEntry> EventQueue; 

	////////////////////////////////////////////////////////////////////

	
	struct EventConnection{
		IEventEmitter* emitter;
		IEventListener* listener;

		EventConnection(IEventEmitter* emitter=NULL, IEventListener* listener=NULL) : emitter(emitter), listener(listener){
		}
	};

	typedef std::vector<EventConnection> EventConnectionList;

	typedef std::vector<IEventListener*> EventListenerList;

	struct EventListeners{
		/** List of listneers subscribed to a specific emitter */
		EventConnectionList connectedListeners;

		/* List of listeners subscribed to all emitters */
		EventListenerList unconnectedListeners;
	};


	struct RegisteredEvent{
		RegisteredEvent(const wt::EvtType eventType) : eventType(eventType){
		}

		const wt::EvtType eventType;
		EventListeners listeners[eCONNECTION_MAX];
	};

	typedef std::map<wt::EvtType::HashValue, RegisteredEvent> EventConnectionTable;

private:
	RegisteredEvent* findRegisteredEvent(const wt::EvtType& type){
		EventConnectionTable::iterator iter = mEventConnections.find(type.getValue());

		return iter == mEventConnections.end() ? NULL : &iter->second;
	}

public:
	void registerEvent(const wt::EvtType& type){
		// Already registered event of this type?
		WT_ASSERT(findRegisteredEvent(type) == NULL,
			"Event of type \"%s\" already registered", type.c_str());

		mEventConnections.insert( std::make_pair(type.getValue(), RegisteredEvent(type)) );
	}

	void unregisterEvent(const wt::EvtType& type){
		EventConnectionTable::iterator iter = mEventConnections.find(type.getValue());

		WT_ASSERT(iter != mEventConnections.end(),
			"Event of type \"%s\" not registered", type.c_str());

		mEventConnections.erase(iter);
	}


	struct ListenerQuery{
		bool global;
		ConnectionType connection;
		bool connected;
		RegisteredEvent* eventReg;
	};

	typedef std::vector<ListenerQuery> ListenerQueryList;

	ListenerQueryList findListener(IEventListener* listener){
		ListenerQueryList res;

		// Check global listeners
		for(uint32_t i=0; i<eCONNECTION_MAX; i++){
			EventListenerList::iterator iter = std::find(mGlobalListeners[i].begin(), mGlobalListeners[i].end(), listener);

			if(iter != mGlobalListeners[i].end()){
				ListenerQuery query;

				// Found the listener
				query.connected = false;
				query.connection = static_cast<ConnectionType>(i);
				query.global = true;

				res.push_back(query);

				return res;
			}
		}


		for(EventConnectionTable::iterator i=mEventConnections.begin(); i != mEventConnections.end(); i++){
			RegisteredEvent& eventReg = i->second;

			for(uint32_t j=0; j<eCONNECTION_MAX; j++){
				EventListenerList::iterator k = std::find(eventReg.listeners[j].unconnectedListeners.begin(), eventReg.listeners[j].unconnectedListeners.end(), listener);

				if(k != eventReg.listeners[j].unconnectedListeners.end()){
					ListenerQuery query;

					// Found the listener
					query.connected = false;
					query.connection = static_cast<ConnectionType>(j);
					query.global = false;
					query.eventReg = &eventReg;

					res.push_back(query);
				}

				for(EventConnectionList::iterator k = eventReg.listeners[j].connectedListeners.begin(); k!=eventReg.listeners[j].connectedListeners.end(); k++){
					if((*k).listener == listener){
						ListenerQuery query;

						// Found the listener
						query.connected = true;
						query.connection = static_cast<ConnectionType>(j);
						query.global = false;
						query.eventReg = &eventReg;

						res.push_back(query);
					}
				}
			}
		}

		return res;
	}

	void registerListener(IEventListener* listener, const wt::EvtType& type="", ConnectionType connectionType=eCONNECTION_DIRECT, IEventEmitter* emitter=NULL){
		const bool global = type == "";

		if(global){
			WT_ASSERT(findListener(listener).empty(),
				"Listener %p already registered", listener);

			// Global listener
			mGlobalListeners[connectionType].push_back(listener);
		}
		else{
			RegisteredEvent* eventReg = findRegisteredEvent(type);

			WT_ASSERT(eventReg != NULL,
				"Error");

			ListenerQueryList query = findListener(listener);

			if(emitter == NULL){
				// Listener registered somewhere?
				for(ListenerQueryList::iterator iter=query.begin(); iter!=query.end(); iter++){
					// Registered as 'unconnected' ?
					if(!(*iter).connected){
						// Already registered to event of this type ?
						WT_ASSERT((*iter).eventReg->eventType != type,
							"Listener %p already registered for event of type \"%s\"", listener, type.c_str());
					}
				}
				// Unconnected listener
				eventReg->listeners[connectionType].unconnectedListeners.push_back(listener);
			}
			else{
				// Listener registered somewhere?
				for(ListenerQueryList::iterator iter=query.begin(); iter!=query.end(); iter++){
					// Registered as 'connected' ?
					if((*iter).connected){
						// Already registered to event of this type ?
						WT_ASSERT((*iter).eventReg->eventType != type,
							"Listener %p already registered for event of type \"%s\"", listener, type.c_str());
					}
				}

				eventReg->listeners[connectionType].connectedListeners.push_back( EventConnection(emitter, listener) );
			}
		}
	}

	void unregisterListener(IEventListener* listener, wt::EvtType type="", IEventEmitter* emitter=NULL){
		// Get a list of registrations for this listener
		ListenerQueryList queryList = findListener(listener);
		WT_ASSERT(!queryList.empty(), "Listener %p not registered for any events");

		for(ListenerQueryList::iterator iter=queryList.begin(); iter!=queryList.end(); iter++){
			ListenerQuery query = *iter;

			if(query.global){
				// Remove it from a global listener list
				EventListenerList& list = mGlobalListeners[query.connection];
				list.erase( std::find(mGlobalListeners[query.connection].begin(), mGlobalListeners[query.connection].end(), listener) );
			}
			else{
				// Remove it from a 'connected' specialized list
				if(query.connected){
					EventConnectionList& l = query.eventReg->listeners[query.connection].connectedListeners;

					for(EventConnectionList::iterator iter=l.begin(); iter!=l.end(); iter++){
						if((*iter).listener == listener){
							l.erase(iter);
							break;
						}
					}
				}
				else if(emitter == NULL){
					// Remove it from a 'unconnected' specialized list
					EventListenerList& l = query.eventReg->listeners[query.connection].unconnectedListeners;
					l.erase( std::find(l.begin(), l.end(), listener) );
				}
			}
		}
	}

	bool handleEvent(const wt::EvtPtr evt, EventListenerList& list){
		for(EventListenerList::iterator iter=list.begin(); iter!=list.end(); iter++){
			bool consumed = (*iter)->handleEvent(evt);

			if(consumed){
				return consumed;
			}
		}

		return false;
	}

	bool handleEvent(const wt::EvtPtr evt, EventConnectionList& list, IEventEmitter* emitter){
		for(EventConnectionList::iterator iter=list.begin(); iter!=list.end(); iter++){
			if((*iter).emitter == emitter){

				bool consumed = (*iter).listener->handleEvent(evt);
				
				if(consumed){
					return consumed;
				}
			}
		}

		return false;
	}

	bool handleEvent(const wt::EvtPtr evt, IEventEmitter* emitter, ConnectionType connectionType){
		RegisteredEvent* eventReg = findRegisteredEvent(evt->getType());

		WT_ASSERT(eventReg != NULL,
			"Attempting to process un-registered event of type \"%s\"", evt->getType().c_str());

		bool consumed = false;

		// Global directly connected listeners
		if( handleEvent(evt, mGlobalListeners[connectionType]) ){
			// Event consumed
			return consumed;
		}
		
		// Handle directly connected, emitter-unconnected, specialized listeners
		if( handleEvent(evt, eventReg->listeners[connectionType].unconnectedListeners) ){
			// Event consumed
			return consumed;
		}

		// If event came from an emitter..
		if(emitter){
			// Handle directly connected, emitter-connected, specialized listeners
			if( handleEvent(evt, eventReg->listeners[connectionType].connectedListeners, emitter) ){
				// Event consumed
				return consumed;
			}
		}

		return false;
	}

	void emit(const wt::EvtPtr evt, IEventEmitter* emitter=NULL){
		if( handleEvent(evt, emitter, eCONNECTION_DIRECT) ){
			// Event consumed
			return;
		}
	
		// Queue the event
		mEventQueue.push( EventQueueEntry(evt, emitter) );
	}

	void update(){
		uint32_t numEvents = mEventQueue.size();

		while(numEvents--){
			EventQueueEntry entry = mEventQueue.front();
			mEventQueue.pop();

			if( handleEvent(entry.event, entry.emitter, eCONNECTION_QUEUED) ){
				// Event consumed
				continue;
			}
		}
	}

private:
	EventListenerList mGlobalListeners[eCONNECTION_MAX];
	EventConnectionTable mEventConnections;
	EventQueue mEventQueue;
};

class AEventEmitter : public IEventEmitter{
public:
	AEventEmitter (EventManager* manager){
	}

	void emitEvent(const wt::EvtPtr evt){
	}

	bool isHooked() const{
		return false;
	}

	void hook(){
	}

	void unhook(){
	}

private:
};

//////////////////////

class DummyEvent : public wt::Event{
public:
	static wt::EvtType type;

	const wt::EvtType& getType() const{

		return type;
	}
};

wt::EvtType DummyEvent::type = "Dummy";

class DummyListener : public IEventListener{
public:
	wt::String mName;
	DummyListener(const char* name) : mName(name){
	}
	
	bool handleEvent(const wt::EvtPtr evt){
		LOG("Event %s", mName.c_str());
		return false;
	}
};


class DummyEmitter : public IEventEmitter{
public:
};

int main(){
	EventManager mngr;

	mngr.registerEvent(DummyEvent::type);

	DummyEmitter emitter;
	DummyListener l("");

	mngr.registerListener(&l,
		DummyEvent::type, EventManager::eCONNECTION_DIRECT);

	mngr.registerListener(&l,
		DummyEvent::type, EventManager::eCONNECTION_DIRECT, &emitter);

	mngr.unregisterListener(&l, "", &emitter);
	//mngr.registerListener(&l,
	//	DummyEvent::type, EventManager::eCONNECTION_DIRECT, &emitter);


	//mngr.registerListener(&l,
	//	DummyEvent::type, EventManager::eCONNECTION_DIRECT);
/*
	mngr.registerListener(&l,
		DummyEvent::type, EventManager::eCONNECTION_DIRECT, &emitter);

	mngr.registerListener(&l,
		DummyEvent::type, EventManager::eCONNECTION_DIRECT, &emitter);*/


	LOG("1");
	
	mngr.emit( new DummyEvent );

	LOG("2");

	mngr.emit( new DummyEvent, &emitter);

	LOG("3");

	mngr.update();

	LOG("4");

	mngr.unregisterEvent(DummyEvent::type);
}

#endif