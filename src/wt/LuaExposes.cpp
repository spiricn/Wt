#include "wt/stdafx.h"

#include "wt/Camera.h"
#include "wt/Lua.h"

#define TD_TRACE_TAG "LuaExposes"

namespace wt{

using namespace Lua;

namespace math{

	void Camera::luaPitch(LuaObject deg){
		WT_ASSERT(deg.IsNumber(), "Not a float number");

		pitch( deg.ToNumber() );
	}

	void Camera::luaYaw(LuaObject deg){
		WT_ASSERT(deg.IsNumber(), "Not a float number");

		yaw( deg.ToNumber() );
	}

	void Camera::luaLookAt(LuaObject pos){
		glm::vec3 position;
		if(Lua::luaConv(pos, position)){
			lookAt(position);
		}
		else{
			// TODO handle
		}
	}

	void Camera::luaSetPosition(LuaObject pos){
		glm::vec3 position;
		if(Lua::luaConv(pos, position)){
			setPosition(position);
		}
		else{
			// TODO handle
		}
	}

	void Camera::luaOrbit(LuaObject lrtr, LuaObject lpos){
		glm::vec3 rtr, pos;

		if(Lua::luaConv(lrtr, rtr) && Lua::luaConv(lpos, pos)){
			orbit(rtr[0], rtr[1], rtr[2], pos);
		}
		else{
			// TODO handle
		}
	}

	void Camera::expose(LuaObject& meta){
		meta.RegisterObjectDirect("pitch", (Camera*)0, &Camera::luaPitch);
		meta.RegisterObjectDirect("yaw", (Camera*)0, &Camera::luaYaw);
		meta.RegisterObjectDirect("setPosition", (Camera*)0, &Camera::luaSetPosition);
		meta.RegisterObjectDirect("lookAt", (Camera*)0, &Camera::luaLookAt);
		meta.RegisterObjectDirect("orbit", (Camera*)0, &Camera::luaOrbit);

	}

}; // </math>

}; // </wt>
