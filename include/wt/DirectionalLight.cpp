#include "wt/stdafx.h"

#include "wt/DirectionalLight.h"

#include "wt/Lua.h"

namespace wt
{

void DirectionalLight::Desc::deserialize(lua::State* luaState, const LuaPlus::LuaObject& src){
	ALight::Desc::deserialize(luaState, src);

	if(!lua::luaConv(src.Get("direction"), direction)){
		// TODO handle
	}	
}

void DirectionalLight::Desc::serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const{
	ALight::Desc::serialize(luaState, dst);

	LuaObject luaDir = luaState->newTable();

	lua::luaConv(direction, luaDir);
	dst.Set("direction", luaDir);
}

DirectionalLight::Desc::Desc() : direction(0.0f, -1.0f, 0.0f){
}

const DirectionalLight::Desc& DirectionalLight::getDesc() const{
	return mDesc;
}

DirectionalLight::Desc& DirectionalLight::getDesc(){
	return mDesc;
}

DirectionalLight::DirectionalLight(Scene* parent, uint32_t id) : ALight(parent, id, ALight::eTYPE_DIRECTIONAL){
}


} // </wt>