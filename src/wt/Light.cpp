#include "wt/stdafx.h"

#include "wt/Light.h"
#include "wt/Color.h"
#include "wt/Lua.h"

namespace wt
{

using namespace LuaPlus;

ALight::ALight(const Color& color, float ambientIntesity, float diffuseItensity) : 
	mColor(color), mAmbientIntesity(ambientIntesity), mDiffuseItensity(diffuseItensity), mActive(true){
}

ALight::~ALight(){
}

void ALight::serialize(lua::State* luaState, LuaPlus::LuaObject& dst){
	LuaObject luaColor = luaState->newTable();

	lua::luaConv(mColor, luaColor);
	dst.Set("color", luaColor);

	dst.Set("ambientIntensity", mAmbientIntesity);

	dst.Set("diffuseIntensity", mDiffuseItensity);
}

void ALight::deserialize(lua::State* luaState, const LuaPlus::LuaObject& src){
	WT_ASSERT(src.IsTable()
		&& src.Get("ambientIntensity").IsNumber()
		&& src.Get("diffuseIntensity").IsNumber()
		&& src.Get("color").IsTable(),
		"Invalid light lua table");

	if(!lua::luaConv(src.Get("color"), mColor)){
		// TODO handle
	}

	if(!lua::luaConv(src.Get("ambientIntensity"), mAmbientIntesity)){
		// TODO handle
	}

	if(!lua::luaConv(src.Get("diffuseIntensity"), mDiffuseItensity)){
		// TODO handle
	}
}


DirectionalLight::DirectionalLight(const Color& color, float ambientIntesity,
	float diffuseItensity, const glm::vec3& dir) : ALight(color, ambientIntesity, diffuseItensity),
	mDirection(dir){
}

void DirectionalLight::deserialize(lua::State* luaState, const LuaPlus::LuaObject& src){
	ALight::deserialize(luaState, src);

	if(!lua::luaConv(src.Get("direction"), mDirection)){
		// TODO handle
	}	
}

void DirectionalLight::serialize(lua::State* luaState, LuaPlus::LuaObject& dst){
	ALight::serialize(luaState, dst);

	LuaObject luaDir = luaState->newTable();

	lua::luaConv(mDirection, luaDir);
	dst.Set("direction", luaDir);
}

PointLight::PointLight (const Color& color, float ambientIntesity,
	float diffuseItensity, const glm::vec3& pos) : ALight(color, ambientIntesity, diffuseItensity),
	mPosition(pos){
		mAttenuation.constant = 0.0f;
		mAttenuation.linear = 0.8f;
		mAttenuation.exponential = 0.00f;
}

void PointLight::deserialize(lua::State* luaState, const LuaPlus::LuaObject& src){
	ALight::deserialize(luaState, src);
	if(!lua::luaConv(src.Get("position"), mPosition)){
		// TODO handle
	}

	lua::luaConv(src.Get("attenConstat"), mAttenuation.constant);

	lua::luaConv(src.Get("attenLinear"), mAttenuation.linear);

	lua::luaConv(src.Get("attenExponential"), mAttenuation.exponential);
}

void PointLight::serialize(lua::State* luaState, LuaPlus::LuaObject& dst){
	ALight::serialize(luaState, dst);

	LuaObject luaPos = luaState->newTable();

	lua::luaConv(mPosition, luaPos);
	dst.Set("position", luaPos);

	dst.Set("attenConstat", mAttenuation.constant);

	dst.Set("attenLinear", mAttenuation.linear);

	dst.Set("attenExponential", mAttenuation.exponential);
}

float PointLight::calculateBoundingSphere(){
	float maxChannel = glm::max(glm::max(mColor.mRed, mColor.mGreen), mColor.mBlue);
	// TODO probably wrong, we need to account for attenuation as well
   	float c = maxChannel * mDiffuseItensity;
   	return (8.0f * glm::sqrt(c) + 1.0f);
}
SpotLight::SpotLight(const Color& color, float ambientIntesity,
	float diffuseItensity, const glm::vec3& pos) : PointLight(color, ambientIntesity, diffuseItensity, pos){
}

} // </wt>
