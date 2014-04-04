#ifndef WT_EVENTEMITTER_H
#define WT_EVENTEMITTER_H

#include "wt/EventManager.h"

namespace wt{


class EventEmitter{
private:
	EventManager* mManager;
	bool mHooked;
	typedef std::vector<EventType> EventList;
	EventList mEvents;

public:
	EventEmitter() : mHooked(false), mManager(NULL){
	}

	~EventEmitter(){
		for(EventList::iterator i=mEvents.begin(); i!=mEvents.end(); i++){
			mManager->unregisterEvent(*i);
		}
	}

	void hook(EventManager* manager, uint32_t numEvents, ...){
		if(mHooked){
			return;
		}

		va_list vlist;
		va_start(vlist, numEvents);

		for(uint32_t i=0; i<numEvents; i++){
			EventType type = va_arg(vlist, EventType);
			mEvents.push_back(type);

			manager->registerEvent(type);
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