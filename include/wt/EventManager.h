#ifndef WT_EVENTMANAGER_H
#define WT_EVENTMANAGER_H


#include "wt/stdafx.h"

#include "wt/Exception.h"

#include "wt/EventListener.h"
#include "wt/Event.h"
#include <td/td.h>
#include "wt/Defines.h"
#include "wt/Mutex.h"

namespace wt{


class ACallback{
public:
	virtual ~ACallback(){
	}

	virtual void handleCallback() = 0;

}; // </ACallback>

typedef wt::Sp<ACallback> CallbackPtr;

template<class T>
class MemberCallback : public ACallback{
public:
	typedef void (T::*MemberCallbackFnc)();

private:
	T* mInstance;
	MemberCallbackFnc mCallback;

public:
	MemberCallback(T* instance, MemberCallbackFnc callback) : mInstance(instance), mCallback(callback){
	}

	void handleCallback(){
		(mInstance->*(mCallback))();
	}

}; // </MemberCallback>


class EventManager : public ALuaObject{
WT_DISALLOW_COPY(EventManager)

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
		};

	private:
		Type mType;

	public:	
		ARegisteredEvent(Type type);

		Type getType() const;

		virtual void queueFromScript(LuaObject& data) = 0;

	}; // </ARegisteredEvent>

	
	class InternalEventReg : public ARegisteredEvent{
	public:
		InternalEventReg();

		void queueFromScript(LuaObject& data);

	}; // </InternalEventReg>



	class ScriptEventReg : public ARegisteredEvent{
	private:
		EvtType mEventType;
		EventManager* mEventManager;
	public:
		virtual ~ScriptEventReg(){
		}

		ScriptEventReg(EventManager* manager, const EvtType& eventType) : mEventManager(manager), mEventType(eventType), ARegisteredEvent(eEVENT_SCRIPT){
		}

		void queueFromScript(LuaObject& data){
			mEventManager->queueEvent( new ScriptEvent(mEventType, data) );
		}

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


private:
	static const char* TAG;

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

	// used for thread safe event-queue
	Mutex mMutex;

	void registerEvent(const EvtType& type, ARegisteredEvtPtr evt);

	
public:
	EventManager();

	virtual ~EventManager();

	void addScriptListener(const EvtType& eventType, LuaObject& callback);

	inline bool isRegistered(const EvtType& type);

	void registerGlobalListener(EventListener* listener);

	void registerInternalEvent(const EvtType& eventType);

	void unregisterInternalEvent(const EvtType& eventType);

	void registerScriptEvent(const EvtType& eventType);

	template<class T>
	void registerSharedEvent(const EvtType& eventType){
		registerEvent(eventType, new SharedEventReg<T>);
	}

	void unregisterGlobalListener(EventListener* listener);

	void unregisterListener(EventListener* listener, const EvtType& eventType);

	void registerListener(EventListener* listener, const EvtType& eventType);

	void registerCallback(CallbackPtr callback, const EvtType& eventType, bool filtered=false, uint32_t filterData=0);

	void unregisterListener(EventListener* listener);

	void queueEvent(EvtPtr evt);

	void tick();

	virtual bool dispatchEvent(EventListener* listener, EvtPtr evt){
		return listener->handleEvent(evt);
	}

	virtual bool dispatchEvent(Sp<ScriptEventListener> listener, EvtPtr evt){
		return listener->handleEvent(evt);
	}

	/**********************/
	/**** Lua bindings ****/
	/**********************/

	void lua_queueEvent(const char* type, LuaObject data);

	void lua_registerScriptEvent(const char* type);

	void lua_addScriptListener(const char* eventType, LuaObject callback);

	void expose(LuaObject& meta);

}; // </EventManager>

}; // </wt>

#endif
