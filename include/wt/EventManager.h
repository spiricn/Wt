#ifndef WT_EVENTMANAGER_H
#define WT_EVENTMANAGER_H

#include "wt/stdafx.h"

#include "wt/Exception.h"
#include "wt/EventListener.h"
#include "wt/Event.h"
#include "wt/ScriptEvent.h"
#include "wt/Defines.h"
#include "wt/Mutex.h"
#include "wt/lua/State.h"
#include "wt/MemberCallback.h"

namespace lua
{

class State;

} // </lua>

namespace wt
{

class EventManager{
public:
	class ARegisteredEvent{
	public:
		enum Type{
			// defined by code, can not be triggered from script
			eEVENT_INTERNAL,

			// defined by script and can be triggered both from script and code
			eEVENT_SCRIPT,

			// defined by code and can be triggered both from script and code
			eEVENT_SHARED,
		}; // </Type>

		ARegisteredEvent(Type type);

		Type getType() const;

		virtual void queueFromScript(LuaObject& data) = 0;

	private:
		Type mType;
	}; // </ARegisteredEvent>

	
	class InternalEventReg : public ARegisteredEvent{
	public:
		InternalEventReg();

		void queueFromScript(LuaObject& data);

	}; // </InternalEventReg>



	class ScriptEventReg : public ARegisteredEvent{
	public:
		virtual ~ScriptEventReg(){
		}

		ScriptEventReg(EventManager* manager, const EvtType& eventType) : mEventManager(manager), mEventType(eventType), ARegisteredEvent(eEVENT_SCRIPT){
		}

		void queueFromScript(LuaObject& data){
			mEventManager->queueEvent( new ScriptEvent(mEventType, data) );
		}

	private:
		EvtType mEventType;
		EventManager* mEventManager;

	}; // </ScriptEventReg>

	template<class T>
	class SharedEventReg : public ARegisteredEvent{
	public:
		SharedEventReg () : ARegisteredEvent(eEVENT_SHARED){
		}

		void queueFromScript(LuaObject& data){
			queueEvent( new T(data) );
		}

	}; // </SharedEventReg>

	typedef Sp<ARegisteredEvent> ARegisteredEvtPtr;

public:
	EventManager(lua::State* luaState);

	virtual ~EventManager();

	bool addScriptListener(const char*, LuaObject callback);

	inline bool isRegistered(const EvtType& type);

	void registerGlobalListener(EventListener* listener);

	void registerListener(EventListener* listener, const EvtType& eventType);

	void registerInternalEvent(const EvtType& eventType);

	void unregisterInternalEvent(const EvtType& eventType);

	void registerScriptEvent(const char* eventType);

	template<class T>
	void registerSharedEvent(const EvtType& eventType){
		registerEvent(eventType, new SharedEventReg<T>);
	}

	void unregisterGlobalListener(EventListener* listener);

	void unregisterListener(EventListener* listener, const EvtType& eventType);

	void registerCallback(CallbackPtr callback, const EvtType& eventType, bool filtered=false, uint32_t filterData=0);

	void unregisterListener(EventListener* listener);

	void queueEvent(EvtPtr evt);

	void queueEvent(const char* type, LuaObject data);

	void fireEvent(EvtPtr evt);

	void tick();

	void generateMetaTable();

private:
	struct RegisteredCallback{
		CallbackPtr callback;
		uint32_t filterData;
		bool isFiltered;

		RegisteredCallback(CallbackPtr callback, uint32_t filterData, bool isFiltered) : callback(callback), filterData(filterData), isFiltered(isFiltered){
		}
	}; // </RegisteredCallback>;

	typedef Sp<ACallback> CallbackPtr;

	typedef std::list<RegisteredCallback> CallbackList;
	typedef std::map<uint32_t, CallbackList> EvtCallbackTable;

	typedef std::list<EventListener*> ListenerList;
	typedef std::map<uint32_t, ListenerList> EvtListenerTable;
	typedef std::list<EvtPtr> EventList;
	typedef std::multimap<EvtType, Sp<ScriptEventListener>> ScriptListenerMap;
	typedef std::multimap<EvtType, ARegisteredEvtPtr> RegisteredEvtMap;

private:

	void registerEvent(const EvtType& type, ARegisteredEvtPtr evt);

private:
	
	RegisteredEvtMap mRegisteredEvents;

	/** list of active queued events */
	EventList mEventList;

	/** table used for mapping an event to a SCRIPT callback */
	ScriptListenerMap mScriptListeners;

	/** table used for mapping an event to a CODE listeners */
	EvtListenerTable mEvtListenerTable;

	/** list of "global" listeners (listening to all events) */
	ListenerList mGlobalListeners;
	
	EvtCallbackTable mEvtCallbackTable;

	lua::State* mLuaState;

	// used for thread safe event-queue
	Mutex mMutex;

}; // </EventManager>

}; // </wt>

#endif
