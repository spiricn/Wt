/**
 * @file IEventListener.h
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */
 
#ifndef WT_IEVENTLISTENER_H
#define WT_IEVENTLISTENER_H

#include <wt/AEvent.h>

namespace wt
{

class IEventListener{
public:
	virtual bool handleEvent(const EventPtr event){
		return true;
	}

	virtual ~IEventListener(){
	}
}; // </IEventListener>

} // </wt>

#endif // </WT_IEVENTLISTENER_H>
