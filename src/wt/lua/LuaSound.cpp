#include "wt/stdafx.h"

#include "wt/lua/LuaSound.h"
#include "wt/lua/LuaModule.h"
#include "wt/Sound.h"

#define GET_THIS Sound* thiz = dynamic_cast<Sound*>( static_cast<ASceneActor*>( ptr ) )

#define TD_TRACE_TAG "LuaSound"

namespace wt
{

namespace lua
{

void Sound_expose(LuaObject obj){
	MODULE_EXPOSE(obj, Sound_setVolume);
}

void Sound_setVolume(void* ptr, float volume){
	GET_THIS;

	thiz->getSound()->setVolume(volume);
}

} // </lua>

} // </wt>

