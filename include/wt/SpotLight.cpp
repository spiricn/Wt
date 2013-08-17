#include "wt/stdafx.h"

#include "wt/SpotLight.h"
#include "wt/Lua.h"

namespace wt
{

SpotLight::Desc::Desc(){
	// TODO
}

void SpotLight::Desc::deserialize(lua::State* luaState, const LuaPlus::LuaObject& src){
	ALight::Desc::deserialize(luaState, src);
	if(!lua::luaConv(src.Get("position"), position)){
		// TODO handle
	}

	lua::luaConv(src.Get("attenConstat"), attenuation.constant);

	lua::luaConv(src.Get("attenLinear"), attenuation.linear);

	lua::luaConv(src.Get("attenExponential"), attenuation.quadratic);

	lua::luaConv(src.Get("cutoff"), cutoff);
}

void SpotLight::Desc::serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const{
	ALight::Desc::serialize(luaState, dst);

	LuaObject luaPos = luaState->newTable();

	lua::luaConv(position, luaPos);
	dst.Set("position", luaPos);

	dst.Set("attenConstat", attenuation.constant);

	dst.Set("attenLinear", attenuation.linear);

	dst.Set("attenExponential", attenuation.quadratic);

	dst.Set("cutoff", cutoff);
}

const SpotLight::Desc& SpotLight::getDesc() const{
	return mDesc;
}

SpotLight::Desc& SpotLight::getDesc(){
	return mDesc;
}

SpotLight::SpotLight(Scene* parent, uint32_t id) : ALight(parent, id, ALight::eTYPE_SPOT){
}

} // </wt>