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

Window::Window(ID id, EventManager* eventManager, AGameInput* input) : mId(id), mRootLayout(NULL){
	// TODO should be created dynamically
	mRootLayout = new Layout(NULL, eventManager, input);
}

Window::~Window(){
	delete mRootLayout;
}

Window::ID Window::getId() const{
	return mId;
}

Layout* Window::getRootLayout(){
	return mRootLayout;
}

void Window::draw(){
	mRootLayout->redraw();
}

void Window::setPosition(const glm::vec2& position){
	mRect.x = position.x;
	mRect.y = position.y;

	mRootLayout->setPosition(position);
}

bool Window::handleEvent(const EventPtr evt){
	return mRootLayout->handleEvent(evt);
}

const Rect& Window::getRect() const{
	return mRect;
}

Texture2D* Window::getTexture(){
	return mRootLayout->getCanvas().getTexture();
}

void Window::setSize(const glm::vec2& size){
	mRect.width = size.x;
	mRect.height = size.y;
	mRootLayout->setSize(size);
}

} // </gui>
	
} // </wt>

