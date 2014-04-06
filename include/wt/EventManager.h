/**
 * @file EventManager.h
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */
 
#ifndef WT_EVENTMANAGER_H
#define WT_EVENTMANAGER_H

#include "wt/AEvent.h"
#include "wt/IEventListener.h"
#include "wt/IEventEmitter.h"
#include "wt/lua/State.h"
#include "wt/MemberCallback.h"

namespace wt
{

class EventManager{
public:
	enum ConnectionType{
		eCONNECTION_DIRECT,
		eCONNECTION_QUEUED,
		eCONNECTION_MAX
	}; // </ConnectionType>

public:
	EventManager(lua::State* luaState=NULL);

	~EventManager();

	void registerListener(IEventListener* listener, const EventType& type="", ConnectionType connectionType=eCONNECTION_QUEUED, IEventEmitter* emitter=NULL, bool autoDestroy=false);

	void unregisterListener(IEventListener* listener, EventType type="", IEventEmitter* emitter=NULL);

	void emit(const EventPtr evt, IEventEmitter* emitter=NULL);

	void update();

	void registerEvent(const EventType& type);

	void unregisterEvent(const EventType& type);

	lua::State* getLuaState() const;

	template<class T>
	void registerCallback(T* instance, void (T::*callbackFnc)(), const EventType& type="", IEventEmitter* emitter=NULL, ConnectionType connectionType=eCONNECTION_QUEUED){
		registerListener(new MemberCallback<T>(instance, callbackFnc),
				type, connectionType, emitter, true);
	}

private:
	struct RegisteredListener{
		bool autoDestroy;
		IEventListener* listener;

		RegisteredListener(IEventListener* listener, bool autoDestroy) : listener(listener), autoDestroy(autoDestroy){
		}

		bool operator==(const IEventListener* other) const{
			return listener == other;
		}

	}; // </RegisteredListener>

	struct EventQueueEntry{
		const EventPtr event;
		IEventEmitter* emitter;

		EventQueueEntry(const EventPtr event, IEventEmitter* emitter) : emitter(emitter), event(event){
		}
	}; // </EventQueueEntry>

	typedef std::vector<EventType> EventTypeList;

	typedef std::queue<EventQueueEntry> EventQueue; 

	typedef std::vector<IEventListener*> EventListenerList;

	typedef std::vector<RegisteredListener> RegisteredListenerList;

	struct EventConnection{
		IEventEmitter* emitter;
		IEventListener* listener;

		EventConnection(IEventEmitter* emitter=NULL, IEventListener* listener=NULL) : emitter(emitter), listener(listener){
		}
	}; // </EventConnection>

	typedef std::vector<EventConnection> EventConnectionList;

	struct EventListeners{
		/** List of listeners subscribed to a specific emitter of this event */
		EventConnectionList connectedListeners;

		/** List of listeners subscribed to all emitters of this event */
		EventListenerList unconnectedListeners;

	}; // </EventListeners>

	struct RegisteredEvent{
		const EventType eventType;
		EventListeners listeners[eCONNECTION_MAX];

		RegisteredEvent(const EventType eventType) : eventType(eventType){
		}
	}; // </RegisteredEvent>

	typedef std::map<EventType::HashValue, RegisteredEvent> EventConnectionTable;

	struct ListenerQuery{
		bool global;
		ConnectionType connection;
		bool connected;
		RegisteredEvent* eventReg;
	};

	typedef std::vector<ListenerQuery> ListenerQueryList;

private:
	ListenerQueryList findListener(IEventListener* listener);

	RegisteredEvent* findRegisteredEvent(const EventType& type);

	bool handleEvent(const EventPtr evt, EventListenerList& list);

	bool handleEvent(const EventPtr evt, EventConnectionList& list, IEventEmitter* emitter);

	bool handleEvent(const EventPtr evt, IEventEmitter* emitter, ConnectionType connectionType);

	void addListener(IEventListener* listener, bool autoDestroy);

	void removeListener(IEventListener* listener);

private:
	RegisteredListenerList mRegisteredListeners;
	EventListenerList mGlobalListeners[eCONNECTION_MAX];
	EventConnectionTable mEventConnections;
	EventQueue mEventQueue;
	lua::State* mLuaState;
}; // </EventManager>

} // </wt>

#endif // </WT_EVENTMANAGER_H>
