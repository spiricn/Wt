/**
 * @file Window.cpp
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */

#include "wt/stdafx.h"
#include "wt/gui/Window.h"

#define TD_TRACE_TAG "Window"

namespace wt
{

namespace gui
{

Window::Window(ID id, EventManager* eventManager) : Layout(eventManager), mId(id){
}

Window::~Window(){
}

Window::ID Window::getId() const{
	return mId;
}

} // </gui>
	
} // </wt>

