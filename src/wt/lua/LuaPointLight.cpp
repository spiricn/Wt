#include "wt/stdafx.h"

#include "wt/lua/LuaPointLight.h"
#include "wt/lua/LuaModule.h"
#include "wt/PointLight.h"

#define GET_THIS PointLight* thiz = dynamic_cast<PointLight*>( static_cast<ASceneActor*>( ptr ) )

#define TD_TRACE_TAG "LuaPointLight"

namespace wt
{

namespace lua
{

void PointLight_expose(LuaObject obj){
	MODULE_EXPOSE(obj, PointLight_setColor);
	MODULE_EXPOSE(obj, PointLight_setEnabled);
}

void PointLight_setColor(void* ptr, LuaObject color){
	GET_THIS;

	PointLight::Desc desc = ((const PointLight*)thiz)->getDesc();

	luaConv(color, desc.color);

	thiz->setDesc(desc);
}

void PointLight_setEnabled(void* ptr, bool state){
	GET_THIS;

	PointLight::Desc desc = ((const PointLight*)thiz)->getDesc();

	if(desc.enabled != state){
		desc.enabled = state;

		thiz->setDesc(desc);
	}
}

} // </lua>

} // </wt>

