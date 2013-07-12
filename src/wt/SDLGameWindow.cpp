#include "wt/stdafx.h"

#include "wt/SDLGameWindow.h"

#define TD_TRACE_TAG "SDLGameWindow"

namespace wt{

const char* SDLGameWindow::TAG = "SDLGameWindow";

SDL_Surface* mScreen;

bool SDLGameWindow::updateVideoMode(){
	SDL_FreeSurface(mScreen);

	mScreen = SDL_SetVideoMode(mVideoMode.mScreenWidth, mVideoMode.mScreenHeigth,
			32,
			SDL_RESIZABLE|SDL_OPENGL
			|(mVideoMode.mBorderless?SDL_NOFRAME:0)
			|(mVideoMode.mFullscreenEnabled?SDL_FULLSCREEN:0));

	if(mScreen == 0){
		LOGE("SDL_SetVideoMode failed: \"%s\"", SDL_GetError());
		return false;
	}
	else{
		return true;
	}
}

SDLGameWindow::SDLGameWindow() : mScreen(NULL){
}

bool SDLGameWindow::create(const VideoMode& mode){
	mVideoMode=mode;

	_putenv("SDL_VIDEO_CENTERED=1");

	if(SDL_Init(SDL_INIT_VIDEO)==-1){
		LOGE("SDL_Init failed: \"%s\"", SDL_GetError());
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		
	setWindowTitle(mVideoMode.mTitle);
	setVsyncEnabled(mVideoMode.mVerticalSyncEnabled);
	setFullscreenEnabled(mVideoMode.mFullscreenEnabled);

	return updateVideoMode();
}

bool SDLGameWindow::destroy(){
	/*SDL_FreeSurface(mScreen);
	SDL_Quit();*/

	return true;
}

void SDLGameWindow::hook(EventManager* evtManager){
	mEvtManager = evtManager;
	mEvtManager->registerListener(this, WindowSizeChange::TYPE);
}

bool SDLGameWindow::handleEvent(const Sp<Event> evt){
	const WindowSizeChange* e = static_cast<const WindowSizeChange*>(evt.get());

	mVideoMode.mScreenWidth = e->newWidth;
	mVideoMode.mScreenHeigth = e->newHeight;

	return true;
}

void SDLGameWindow::setVsyncEnabled(bool state){
	state=false;
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, mVideoMode.mVerticalSyncEnabled);
}

SDLGameWindow::~SDLGameWindow(){
	//destroy();
}

void SDLGameWindow::swapBuffers(){
	SDL_GL_SwapBuffers();
}

bool SDLGameWindow::isFullscreen() const{
	return mVideoMode.mFullscreenEnabled;
}

void SDLGameWindow::setScreenSize(Uint32 width, Uint32 height){
}
	
void SDLGameWindow::setFullscreenEnabled(bool state){
	mVideoMode.mFullscreenEnabled=state;
	updateVideoMode();
}

void SDLGameWindow::setWindowTitle(const String& title){
	mVideoMode.mTitle = title;
	SDL_WM_SetCaption(title.c_str(), 0);
}

}; // </wt>
