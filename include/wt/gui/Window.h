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

class Window{
public:
	typedef uint32_t ID;

public:

	Window(ID id, EventManager* eventManager, AGameInput* input);

	~Window();

	ID getId() const;

	Layout* getRootLayout();

	void draw();

	bool handleEvent(const EventPtr evt);

	const Rect& getRect() const;

	Texture2D* getTexture();

	void setSize(const glm::vec2& size);

	void setPosition(const glm::vec2& position);

private:
	Layout* mRootLayout;
	ID mId;
	//Canvas mCanvas;
	Rect mRect;
}; // </Window>

} // </gui>

} // </wt>

#endif // </WT_WINDOW_H>
