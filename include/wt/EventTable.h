#ifndef WT_EVENTTABLE_H
#define WT_EVENTTABLE_H

#include "wt/EventManager.h"

namespace wt{


template<class T>
class EventTable : public IEventListener{
friend T;


public:
	typedef bool (T::*EventCallback)(const Sp<AEvent>);
	typedef void (T::*Callback)();




protected:
	struct FilteredCallback{
		enum CallbackType{
			eEVENT_CALLBACK,
			eCALLBACK
		}; // </CallbackType>

		union CallbackFunc{
			EventCallback eventCallback;
			Callback callback;

			CallbackFunc(EventCallback cb) : eventCallback(cb){
			}

			CallbackFunc(Callback cb) : callback(cb){
			}

		} callback;

		CallbackType type;

		uint32_t filter;
		bool isFiltered;

		FilteredCallback(EventCallback callback, bool isFiltered=false, uint32_t filter=0) : callback(callback),
			isFiltered(isFiltered), filter(filter), type(eEVENT_CALLBACK){
		}

		FilteredCallback(Callback callback, bool isFiltered=false, uint32_t filter=0) : callback(callback),
			isFiltered(isFiltered), filter(filter), type(eCALLBACK){
		}
	}; // </FilteredCallback>
	
	typedef std::vector<FilteredCallback> CallbackList;


	void registerCallback(const EventType& type, EventCallback callback){
		registerCallback(type, FilteredCallback(callback) );
	}

	void registerCallback(const EventType& type, Callback callback){
		registerCallback(type, FilteredCallback(callback, false) );
	}

	void registerCallback(const EventType& type, EventCallback callback, uint32_t filter){
		registerCallback(type, FilteredCallback(callback, true, filter) );
	}

	void registerCallback(const EventType& type, Callback callback, uint32_t filter){
		registerCallback(type, FilteredCallback(callback, true, filter) );
	}


	void init(T* instance, EventManager* manager){
		mInstance = instance;
		mEventManager = manager;

		mEventManager->registerGlobalListener(this);
	}

private:
	T* mInstance;

	EventManager* mEventManager;

	typedef std::map<EventType, CallbackList> CallbackTable;

	CallbackTable mTable;

	void registerCallback(const EventType& type, const FilteredCallback& callback){
		if(mTable.find(type) == mTable.end()){
			mTable.insert( std::make_pair(type, CallbackList()) );
		}

		CallbackList& list = mTable[type];
		list.push_back(callback);
	}

public:
	~EventTable(){
		//mEventManager->unregisterGlobalListener(this);
	}

	bool handleEvent(const Sp<AEvent> e){
		CallbackTable::iterator iter = mTable.find(e->getType());
		if(iter != mTable.end()){
			for(CallbackList::iterator i=iter->second.begin(); i!=iter->second.end(); i++){
				if(!i->isFiltered || i->filter == e->getEmitterData()){
					if(i->type == FilteredCallback::eEVENT_CALLBACK){
						return (mInstance->*(i->callback.eventCallback))(e);
					}
					else{
						(mInstance->*(i->callback.callback))();
						return false;
					}
				}
			}
		}

		return false;
	}
};

#define DECLARE_EVENT_TABLE( klass ) EventTable<klass> _mEventTable;

#define INIT_EVENT_TABLE( eventManager ) do{ _mEventTable.init(this, eventManager); }while(0)

#define REGISTER_EVENT_CALLBACK( type, callback ) do{ _mEventTable.registerCallback(type, callback); }while(0)

#define REGISTER_FILTERED_CALLBACK( type, callback, filter) do{ _mEventTable.registerCallback(type, callback, filter); }while(0)

}; // </wt>

#endif // </WT_EVENTTABLE_H>