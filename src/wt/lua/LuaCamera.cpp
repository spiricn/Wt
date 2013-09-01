#include "wt/stdafx.h"

#include "wt/lua/LuaCamera.h"
#include "wt/lua/LuaModule.h"

#include "wt/Camera.h"

#define GET_THIS math::Camera* thiz = static_cast<math::Camera*>( ptr )

#define TD_TRACE_TAG "LuaCamera"

namespace wt
{

namespace lua
{

void Camera_expose(LuaObject obj){
	MODULE_EXPOSE(obj, Camera_lookAt);
}

void Camera_lookAt(void* ptr, LuaObject luaPos){
	GET_THIS;

	glm::vec3 pos;
	luaConv(luaPos, pos);

	thiz->lookAt(pos);
}

} // </lua>

} // </wt>

