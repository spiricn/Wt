/**
 * @file EventManager.cpp
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */

#include "wt/stdafx.h"
#include "wt/EventManager.h"
#include "wt/IEventEmitter.h"
#include "wt/IEventListener.h"

#define TD_TRACE_TAG "EventManager"

namespace wt
{

EventManager::EventManager(lua::State* luaState) : mLuaState(luaState){
}

EventManager::~EventManager(){
	// Delete any unregistered listeners set to auto destroy
	for(RegisteredListenerList::iterator iter=mRegisteredListeners.begin(); iter!=mRegisteredListeners.end(); iter++){
		if((*iter).autoDestroy){
			delete (*iter).listener;
		}
	}
}

void EventManager::addListener(IEventListener* listener, bool autoDestroy){
	mRegisteredListeners.push_back( RegisteredListener(listener, autoDestroy) );
}

void EventManager::removeListener(IEventListener* listener){
	for(RegisteredListenerList::iterator iter=mRegisteredListeners.begin(); iter!=mRegisteredListeners.end(); iter++){
		if((*iter).listener == listener){
			if((*iter).autoDestroy){
				delete (*iter).listener;
			}

			mRegisteredListeners.erase(iter);

			return;
		}
	}
}

void EventManager::registerListener(IEventListener* listener, const EventType& type, ConnectionType connectionType, IEventEmitter* emitter, bool autoDestroy){
	const bool global = type == "";

	bool added = false;

	if(global){
		WT_ASSERT(findListener(listener).empty(),
			"Listener %p already registered", listener);

		// Global listener
		mGlobalListeners[connectionType].push_back( listener );

		added = true;
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

			added = true;
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

			added = true;
		}
	}

	ListenerQueryList query = findListener(listener);
	if(query.size() == 1){
		addListener(listener, autoDestroy);
	}
}

void EventManager::unregisterListener(IEventListener* listener, EventType type, IEventEmitter* emitter){
	// Get a list of registrations for this listener
	ListenerQueryList queryList = findListener(listener);
	WT_ASSERT(!queryList.empty(), "Listener %p not registered for any events");


	bool autoDestroy = false;

	for(ListenerQueryList::iterator iter=queryList.begin(); iter!=queryList.end(); iter++){
		ListenerQuery query = *iter;

		if(query.global){
			// Remove it from a global listener list
			EventListenerList& list = mGlobalListeners[query.connection];

			EventListenerList::iterator iter = std::find(mGlobalListeners[query.connection].begin(), mGlobalListeners[query.connection].end(), listener);

			list.erase(iter);
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

				EventListenerList::iterator iter = std::find(l.begin(), l.end(), listener);

				l.erase(iter);
			}
		}
	}

	queryList = findListener(listener);

	// We can safely remove it from the list	
	if(queryList.empty()){
		removeListener(listener);
	}
}

bool EventManager::handleEvent(const EventPtr evt, EventListenerList& list){
	for(EventListenerList::iterator iter=list.begin(); iter!=list.end(); iter++){
		bool consumed = (*iter)->handleEvent(evt);

		if(consumed){
			return consumed;
		}
	}

	return false;
}

bool EventManager::handleEvent(const EventPtr evt, EventConnectionList& list, IEventEmitter* emitter){
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

bool EventManager::handleEvent(const EventPtr evt, IEventEmitter* emitter, ConnectionType connectionType){
	RegisteredEvent* eventReg = findRegisteredEvent(evt->getType());

	WT_ASSERT(eventReg != NULL,
		"Attempting to process un-registered event of type \"%s\"", evt->getType().c_str());

	bool consumed = false;

	// Global directly connected listeners
	if( handleEvent(evt, mGlobalListeners[connectionType]) ){
		// AEvent consumed
		return consumed;
	}
		
	// Handle directly connected, emitter-unconnected, specialized listeners
	if( handleEvent(evt, eventReg->listeners[connectionType].unconnectedListeners) ){
		// AEvent consumed
		return consumed;
	}

	// If event came from an emitter..
	if(emitter){
		// Handle directly connected, emitter-connected, specialized listeners
		if( handleEvent(evt, eventReg->listeners[connectionType].connectedListeners, emitter) ){
			// AEvent consumed
			return consumed;
		}
	}

	return false;
}

void EventManager::emitEvent(const EventPtr evt, IEventEmitter* emitter){
	const_cast<AEvent*>(evt.get())->setManager(this);
	const_cast<AEvent*>(evt.get())->setEmitter(emitter);


	if( handleEvent(evt, emitter, eCONNECTION_DIRECT) ){
		// AEvent consumed
		return;
	}
	
	// Queue the event
	mEventQueue.push( EventQueueEntry(evt, emitter) );
}

void EventManager::update(){
	uint32_t numEvents = mEventQueue.size();

	while(numEvents--){
		EventQueueEntry entry = mEventQueue.front();
		mEventQueue.pop();

		if( handleEvent(entry.event, entry.emitter, eCONNECTION_QUEUED) ){
			// AEvent consumed
			continue;
		}
	}
}

void EventManager::registerEvent(const EventType& type){
	// Already registered event of this type?
	WT_ASSERT(findRegisteredEvent(type) == NULL,
		"AEvent of type \"%s\" already registered", type.c_str());

	mEventConnections.insert( std::make_pair(type.getValue(), RegisteredEvent(type)) );
}

void EventManager::unregisterEvent(const EventType& type){
	EventConnectionTable::iterator iter = mEventConnections.find(type.getValue());

	WT_ASSERT(iter != mEventConnections.end(),
		"AEvent of type \"%s\" not registered", type.c_str());

	mEventConnections.erase(iter);
}

EventManager::RegisteredEvent* EventManager::findRegisteredEvent(const EventType& type){
	EventConnectionTable::iterator iter = mEventConnections.find(type.getValue());

	return iter == mEventConnections.end() ? NULL : &iter->second;
}

EventManager::ListenerQueryList EventManager::findListener(IEventListener* listener){
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

lua::State* EventManager::getLuaState() const{
	return mLuaState;
}

} // </wt>

