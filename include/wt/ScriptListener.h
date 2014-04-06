/**
 * @file ScriptListener.h
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */
 
#ifndef WT_SCRIPTLISTENER_H
#define WT_SCRIPTLISTENER_H

#include "wt/IEventListener.h"

namespace wt
{

class ScriptEventListener : public IEventListener{
public:
	ScriptEventListener(LuaObject& callback);

	bool handleEvent(const EventPtr event);

private:
	LuaObject mCallback;

}; // </ScriptEventListener>


} // </wt>

#endif // </WT_SCRIPTLISTENER_H>
