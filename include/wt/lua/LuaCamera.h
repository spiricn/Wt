#ifndef WT_LUACAMERA_H
#define WT_LUACAMERA_H

#include "wt/Lua.h"

namespace wt
{

namespace lua
{

void Camera_expose(LuaObject obj);

void Camera_lookAt(void* ptr, LuaObject pos);

} // </lua>

} // </wt>

#endif // </WT_LUACAMERA_H>