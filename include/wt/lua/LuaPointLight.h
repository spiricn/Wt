#ifndef WT_LUAPOINTLIGHT_H
#define WT_LUAPOINTLIGHT_H

#include "wt/Lua.h"

namespace wt
{

namespace lua
{

void PointLight_expose(LuaObject obj);

void PointLight_setColor(void* ptr, LuaObject color);

void PointLight_setEnabled(void* ptr, bool state);

} // </lua>

} // </wt>

#endif // </WT_LUAPOINTLIGHT_H>