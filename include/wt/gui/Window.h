/**
 * @file Window.h
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */
 
#ifndef WT_WINDOW_H
#define WT_WINDOW_H

#include "wt/gui/Layout.h"

namespace wt
{

namespace gui
{

class Window : public Layout{
public:
	typedef uint32_t ID;

	Window(ID id, EventManager* eventManager);

	~Window();

	ID getId() const;

private:
	ID mId;
}; // </Window>

} // </gui>

} // </wt>

#endif // </WT_WINDOW_H>
