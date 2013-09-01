#ifndef WT_LUASOUND_H
#define WT_LUASOUND_H

#include "wt/Lua.h"

namespace wt
{

namespace lua
{

void Sound_expose(LuaObject obj);

void Sound_setVolume(void* ptr, float volume);

} // </lua>

} // </wt>

#endif // </WT_LUASOUND_H>