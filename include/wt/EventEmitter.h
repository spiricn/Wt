#ifndef WT_EVENTEMITTER_H
#define WT_EVENTEMITTER_H

#include "wt/EventManager.h"

namespace wt{


class EventEmitter{
private:
	EventManager* mManager;
	bool mHooked;
	typedef std::vector<EvtType> EventList;
	EventList mEvents;

public:
	EventEmitter() : mHooked(false), mManager(NULL){
	}

	~EventEmitter(){
		for(EventList::iterator i=mEvents.begin(); i!=mEvents.end(); i++){
			mManager->unregisterInternalEvent(*i);
		}
	}

	void hook(EventManager* manager, uint32_t numEvents, ...){
		if(mHooked){
			return;
		}

		va_list vlist;
		va_start(vlist, numEvents);

		for(uint32_t i=0; i<numEvents; i++){
			EvtType type = va_arg(vlist, EvtType);
			mEvents.push_back(type);

			manager->registerInternalEvent(type);
		}
		
		mHooked = true;

		mManager = manager;

		va_end(vlist);
	}

	EventManager* getEventManager() const{
		return mManager;
	}

}; // </EventEmitter>

}; // </wt>

#endif // </WT_EVENTEMITTER_H>