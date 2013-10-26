#include "wt/stdafx.h"

#define USE_SDL_GAME_INPUT

#include "wt/GameInputFactory.h"


#ifdef USE_SDL_GAME_INPUT

#include "wt/SDLGameInput.h"

#endif

namespace wt
{

AGameInput* GameInputFactory::create(){

#if defined(USE_SDL_GAME_INPUT)
	return new SDLGameInput;
#else
	#error No game input module defined
#endif

}


} // </wt>
