#include "wt/stdafx.h"

#include "wt/EventManager.h"

#define TD_TRACE_TAG "EventManager"

namespace wt{


void EventManager::registerCallback(CallbackPtr callback, const EvtType& eventType, bool filtered, uint32_t filterData){
	if(!isRegistered(eventType)){
		WT_THROW("Trying to register a listener for an unregistered event type \"%s\"",
			eventType.c_str()
			);
	}

	CallbackList* callbackList = NULL;

	EvtCallbackTable::iterator i = mEvtCallbackTable.find( eventType.getValue() );
	if(i!=mEvtCallbackTable.end()){
		callbackList = &i->second;
	}
	else{
		// TODO optimize this?
		mEvtCallbackTable.insert( std::make_pair(eventType.getValue(), CallbackList()) );
		callbackList = &mEvtCallbackTable.find( eventType.getValue() )->second;
	}

	callbackList->push_back( RegisteredCallback(callback, filterData, filtered) );
}

EventManager::ARegisteredEvent::ARegisteredEvent(Type type) : mType(type){
}

EventManager::ARegisteredEvent::Type EventManager::ARegisteredEvent::getType() const{
	return mType;
}


EventManager::InternalEventReg::InternalEventReg() : ARegisteredEvent(eEVENT_INTERNAL){
}

void EventManager::InternalEventReg::queueFromScript(LuaObject& data){
	TRACEE("Internal events can not be queued from script");
}

EventManager::EventManager(lua::State* luaState) : mLuaState(luaState){
}

EventManager::~EventManager(){
	mScriptListeners.clear();
}

bool EventManager::addScriptListener(const char* eventType, LuaObject callback){
	if(!isRegistered(eventType)){
		TRACEE("Trying to register a listener for an unregistered event type \"%s\"",
			eventType
		);
		return false;
	}

	if(!callback.IsFunction()){
		TRACEE("Invalid callback function object (got %s instead of function)", callback.TypeName());
		return false;
	}

	mScriptListeners.insert( std::pair<EvtType,
		ScriptEventListener*>(eventType, new ScriptEventListener(callback)) );

	return true;
}

inline bool EventManager::isRegistered(const EvtType& type){
	return mRegisteredEvents.find(type) != mRegisteredEvents.end();
}

void EventManager::registerGlobalListener(EventListener* listener){
	if(std::find(mGlobalListeners.begin(), mGlobalListeners.end(), listener) != mGlobalListeners.end()){
		WT_THROW("Global listener already registered");
	}
	else{
		bool listenerRegistered = false;

		// Check to see if this listener is already registered for some event
		for(EvtListenerTable::iterator iter=mEvtListenerTable.begin(); iter!=mEvtListenerTable.end(); iter++){
			if(std::find(iter->second.begin(), iter->second.end(), listener) != iter->second.end()){
				listenerRegistered = true;
				break;
			}
		}

		if(listenerRegistered){
			WT_THROW("Global listener already registered for an event"); 
		}

		mGlobalListeners.push_back(listener);
	}
}


void EventManager::unregisterGlobalListener(EventListener* listener){
	ListenerList::iterator iter = std::find(mGlobalListeners.begin(), mGlobalListeners.end(), listener);

	if(iter == mGlobalListeners.end()){
		WT_THROW("Global listener not registered");
	}
	else{
		mGlobalListeners.erase(iter);
	}
}

void EventManager::unregisterInternalEvent(const EvtType& eventType){
	WT_ASSERT(isRegistered(eventType),
		"Trying to register an already registered event of type \"%s\"", eventType.getString().c_str());
	
	mRegisteredEvents.erase(eventType);
}

void EventManager::registerEvent(const EvtType& type, ARegisteredEvtPtr evt){
	if(isRegistered(type)){
		WT_THROW("Trying to register an already registered event of type \"%s\"", type.getString().c_str());
	}
	else{
		mRegisteredEvents.insert(
			std::pair<EvtType, ARegisteredEvtPtr>(type, evt)
		);
	}
}

void EventManager::registerInternalEvent(const EvtType& eventType){
	registerEvent(eventType, new InternalEventReg);
}

void EventManager::registerScriptEvent(const char* eventType){
	registerEvent(eventType, new ScriptEventReg(this, eventType) );
}

void EventManager::registerListener(EventListener* listener, const EvtType& eventType){
	if(!isRegistered(eventType)){
		WT_THROW("Trying to register a listener for an unregistered event type \"%s\"",
			eventType.getString().c_str());
	}

	if(std::find(mGlobalListeners.begin(), mGlobalListeners.end(), listener) != mGlobalListeners.end()){
		WT_THROW("Listener already registered as global");
	}

	// find event listener list in the table
	EvtListenerTable::iterator iter = mEvtListenerTable.find(eventType.getValue());

	// if the list for the given event doesn't exist, create it
	if(iter==mEvtListenerTable.end()){
		mEvtListenerTable[eventType.getValue()] = ListenerList();
	}

	// check if the given listener is already registered for the event
	ListenerList& list = mEvtListenerTable[eventType.getValue()];
	for(ListenerList::iterator i=list.begin(); i!=list.end(); i++){
		if(*i==listener){
			WT_THROW("Listener already registered for the event \"%s\"\n", eventType.getString().c_str()
				);
		}
	}
		
	// add the listener to the list
	list.push_back(listener);
}

void EventManager::unregisterListener(EventListener* listener){
	for(EvtListenerTable::iterator i=mEvtListenerTable.begin(); i!=mEvtListenerTable.end(); i++){
		ListenerList::iterator listenerIter = std::find(i->second.begin(), i->second.end(), listener);
		if(listenerIter != i->second.end()){
			i->second.erase(listenerIter);
		}
	}
}

void EventManager::unregisterListener(EventListener* listener, const EvtType& eventType){
	// find event listener list in the table
	EvtListenerTable::iterator listIter = mEvtListenerTable.find(eventType.getValue());

	ListenerList::iterator listenerIter = std::find(listIter ->second.begin(), listIter ->second.end(), listener);
	if(listenerIter == listIter->second.end()){
		WT_THROW(
				"Listener not registered for event \"%s\"\n", eventType.getString().c_str()
		);
	}
	else{
		listIter->second.erase(listenerIter);
	}
}

void EventManager::queueEvent(EvtPtr evt){
	//mMutex.lock();

	if(!isRegistered(evt->getType())){
		WT_THROW(
			"Trying to queue an unregistered event \"%s\"   %d\n",
				evt->getType().getString().c_str(), evt->getType().getValue()
				);
	}
	else{
		mEventList.push_back(evt); // add the event to back of the list
	}

	//mMutex.unlock();
}

void EventManager::tick(){
	// TODO Will cause deadlock if an event handler queues another event
	// Should probably use concurrent queue for this instead

	//mMutex.lock();

	// Using a counter to prevent recursive event adition (i.e. notification of one event causes another to be queued)
	uint32_t maxEvents = mEventList.size();

	while(!mEventList.empty() && maxEvents > 0){
		EvtPtr evt = mEventList.front();
		maxEvents--;

		fireEvent(evt);

		mEventList.pop_front();
	}

	//mMutex.unlock();
}

void EventManager::queueEvent(const char* type, LuaObject data){
	if(!isRegistered(type)){
		TRACEE("Trying to queue an unregistered event of type \"%s\"", type);
		return;
	}
	
	mRegisteredEvents.find(type)->second->queueFromScript(data);
}

void EventManager::fireEvent(EvtPtr evt){
	{ 
		// Callbacks
		EvtCallbackTable::iterator iter = mEvtCallbackTable.find( evt->getType().getValue() );
		if(iter != mEvtCallbackTable.end()){
			for(CallbackList::iterator i=iter->second.begin(); i!=iter->second.end(); i++){
				if(!i->isFiltered  || (i->isFiltered && i->filterData==evt->getEmitterData())){
					i->callback->handleCallback();
				}
			}
		}
	}

	{ 
		// Code listeners
		EvtListenerTable::iterator iter = mEvtListenerTable.find(evt->getType().getValue());

		// Global handlers
		for(ListenerList::iterator i=mGlobalListeners.begin(); i!=mGlobalListeners.end(); i++){
			(*i)->handleEvent(evt);
		}

		// Specialized handlers
		if(iter!=mEvtListenerTable.end()){ // event is being handled by some listeners
			ListenerList& list = iter->second;

			for(ListenerList::iterator i=list.begin(); i!=list.end(); i++){
				if( ! (*i)->handleEvent(evt) ){
					break; // listener consumed the event
				}
			}
		}
	}

	{
		// Script listeners
		ScriptListenerMap::iterator iter = mScriptListeners.find(evt->getType());

		if(iter != mScriptListeners.end()){
			ScriptListenerMap::iterator end = mScriptListeners.upper_bound(iter->first);

			while(iter != end){
				evt->buildLuaData(mLuaState);

				if(!iter->second->handleEvent(evt)){
					break;
				}
				iter++;
			}
		}
	}

}

} // </wt>
