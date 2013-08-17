#include "wt/stdafx.h"

#include "wt/PointLight.h"

#include "wt/Lua.h"

#include "wt/Scene.h"

namespace wt
{

void PointLight::setDesc(const Desc& desc) const{
	getParent()->setPointLightDesc(this, desc);
}

void PointLight::Desc::deserialize(lua::State* luaState, const LuaPlus::LuaObject& src){
	ALight::Desc::deserialize(luaState, src);
	if(!lua::luaConv(src.Get("position"), position)){
		// TODO handle
	}

	lua::luaConv(src.Get("attenConstat"), attenuation.constant);

	lua::luaConv(src.Get("attenLinear"), attenuation.linear);

	lua::luaConv(src.Get("attenExponential"), attenuation.quadratic);
}

void PointLight::Desc::serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const{
	ALight::Desc::serialize(luaState, dst);

	LuaObject luaPos = luaState->newTable();

	lua::luaConv(position, luaPos);
	dst.Set("position", luaPos);

	dst.Set("attenConstat", attenuation.constant);

	dst.Set("attenLinear", attenuation.linear);

	dst.Set("attenExponential", attenuation.quadratic);
}

PointLight::Desc::Desc(){
	// TODO
}

PointLight::PointLight(Scene* parent, uint32_t id) : ALight(parent, id, ALight::eTYPE_POINT){
}

float PointLight::Desc::calculateBoundingSphere() const{
	// Treshold (consider changing this)

	static const float m = 20/255.0f;

	float f = glm::max(diffuseIntensity, ambientIntensity) * glm::max(glm::max(color.red, color.green), color.blue);

	float C = attenuation.constant;

	float L = attenuation.linear;

	float Q = attenuation.quadratic;

	float res = 0.0f;

	if(L != 0.0f && Q != 0.0f){
		float D = glm::sqrt( glm::pow(L, 2.0f) - 4*(C - f/m)*Q );

		float d1 = (-L + D) / ( 2 * Q);

		float d2 = (-L - D) / ( 2 * Q );

		res = glm::max(d1, d2);
	}
	else if(L == 0.0f && Q != 0.0f){
		res = glm::sqrt( (-(C - f/m)) / Q );
	}
	else if(L != 0.0f && Q == 0.0f){
		res = ((f/m) - C) / L;
	}
	else{
		res = FLT_MAX;
	}

	return res;
}

const PointLight::Desc& PointLight::getDesc() const{
	return mDesc;
}

PointLight::Desc& PointLight::getDesc(){
	return mDesc;
}

} // </wt>