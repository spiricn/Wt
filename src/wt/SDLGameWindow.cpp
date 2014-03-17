#include "wt/stdafx.h"

#include "wt/SDLGameWindow.h"

#define TD_TRACE_TAG "SDLGameWindow"

namespace wt
{


void SDLGameWindow::updateVideoMode(){
	if(mScreen){
		SDL_FreeSurface(mScreen);
		mScreen = NULL;
	}

	Uint32 flags = SDL_RESIZABLE|SDL_OPENGL;

	if(mDesc.mode == eMODE_FULLSCREEN){
		flags |= SDL_FULLSCREEN;
	}
	else if(mDesc.mode == eMODE_WINDOWED_BORDERLESS){
		flags |= SDL_NOFRAME;
	}

	mScreen = SDL_SetVideoMode(mDesc.screenWidth, mDesc.screenHeight,
			mDesc.pixelDepth,
			flags
	);

	if(mScreen == NULL){
		WT_THROW("SDL_SetVideoMode failed: \"%s\"", SDL_GetError());
	}
}

SDLGameWindow::SDLGameWindow() : mScreen(NULL){
}

void SDLGameWindow::create(const Desc& desc){
	mDesc = desc;

	_putenv("SDL_VIDEO_CENTERED=1");

	if(SDL_Init(SDL_INIT_VIDEO)==-1){
		WT_THROW("SDL_Init failed: \"%s\"", SDL_GetError());
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		
	setWindowTitle(desc.windowTitle);

	setVsyncEnabled(desc.vsyncEnabled);

	setWindowMode(desc.mode);

	updateVideoMode();
}

void SDLGameWindow::hook(EventManager* evtManager){
	mEvtManager = evtManager;
}

void SDLGameWindow::setVsyncEnabled(bool state){
	mDesc.vsyncEnabled = state;

	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, mDesc.vsyncEnabled);
}

SDLGameWindow::~SDLGameWindow(){
	SDL_FreeSurface(mScreen);
	SDL_Quit();
}

void SDLGameWindow::swapBuffers(){
	SDL_GL_SwapBuffers();
}


const AGameWindow::Desc& SDLGameWindow::getDesc() const{
	return mDesc;
}

void SDLGameWindow::setScreenSize(uint32_t, uint32_t){
	// TODO
	TRACEW("Not implemented");
}

void SDLGameWindow::setWindowMode(AGameWindow::Mode mode){
	mDesc.mode = mode;

	updateVideoMode();
}

void SDLGameWindow::setWindowTitle(const String& title){
	mDesc.windowTitle = title;

	SDL_WM_SetCaption(title.c_str(), 0);
}

}; // </wt>
