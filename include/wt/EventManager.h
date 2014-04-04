/**
 * @file EventManager.h
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */
 
#ifndef WT_EVENTMANAGER_H
#define WT_EVENTMANAGER_H

#include "wt/AEvent.h"
#include "wt/IEventListener.h"
#include "wt/IEventEmitter.h"

namespace wt
{

class EventManager{
public:
	enum ConnectionType{
		eCONNECTION_DIRECT,
		eCONNECTION_QUEUED,
		eCONNECTION_MAX
	};

public:
	void registerListener(IEventListener* listener, const EventType& type="", ConnectionType connectionType=eCONNECTION_DIRECT, IEventEmitter* emitter=NULL);

	void unregisterListener(IEventListener* listener, EventType type="", IEventEmitter* emitter=NULL);

	void emit(const EventPtr evt, IEventEmitter* emitter=NULL);

	void update();

	void registerEvent(const EventType& type);

	void unregisterEvent(const EventType& type);

private:
	struct EventQueueEntry{
		const EventPtr event;
		IEventEmitter* emitter;

		EventQueueEntry(const EventPtr event, IEventEmitter* emitter) : emitter(emitter), event(event){
		}
	};

	typedef std::vector<EventType> EventTypeList;

	typedef std::queue<EventQueueEntry> EventQueue; 

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
		RegisteredEvent(const EventType eventType) : eventType(eventType){
		}

		const EventType eventType;
		EventListeners listeners[eCONNECTION_MAX];
	};

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

private:
	EventListenerList mGlobalListeners[eCONNECTION_MAX];
	EventConnectionTable mEventConnections;
	EventQueue mEventQueue;

}; // </EventManager>

} // </wt>

#endif // </WT_EVENTMANAGER_H>
