#include "wt/stdafx.h"

#include "wt/PointLight.h"

#include "wt/Lua.h"

#include "wt/Scene.h"

namespace wt
{

void PointLight::setDesc(const Desc& desc) const{
	getParent()->setPointLightDesc(this, desc);
}

void PointLight::setTranslation(const glm::vec3& translation){
	Desc desc = getDesc();
	desc.position = translation;

	setDesc(desc);
}

void PointLight::setRotation(const glm::quat&){
}

void PointLight::setScale(const glm::vec3&){
}

void PointLight::getScale(glm::vec3&) const{
}

void PointLight::getTranslation(glm::vec3& result) const{
	Desc desc = getDesc();
	result = desc.position;
}

void PointLight::getRotation(glm::quat&) const{
}

PointLight::Desc::Desc(){
	attenuation.constant = 0.0f;
	attenuation.linear = 0.0f;
	attenuation.quadratic = 0.3f;
}

ATransformable* PointLight::getTransformable() const{
	return const_cast<ATransformable*>( dynamic_cast<const ATransformable*>(this) );
}

PointLight::PointLight(Scene* parent, uint32_t id, const String& name) : ALight(parent, id, ALight::eTYPE_POINT), ASceneActor(parent, ASceneActor::eTYPE_POINT_LIGHT, id, name){
}

float PointLight::Desc::calculateBoundingSphere() const{
	// Treshold (consider changing this)

	static const float m = 10/255.0f;

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


void PointLight::deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src, void* opaque){
	ASceneActor::deserialize(assets, src, opaque);

	Desc desc = getDesc();

	if(!lua::luaConv(src.Get("attenConstat"), desc.attenuation.constant)){
		// TODO
	}

	if(!lua::luaConv(src.Get("attenLinear"), desc.attenuation.linear)){
		// TODO
	}

	if(!lua::luaConv(src.Get("attenExponential"), desc.attenuation.quadratic)){
		// TODO
	}

	if(!lua::luaConv(src.Get("color"), desc.color)){
		// TODO handle
	}

	if(!lua::luaConv(src.Get("ambientIntensity"), desc.ambientIntensity)){
		// TODO handle
	}

	if(!lua::luaConv(src.Get("diffuseIntensity"), desc.diffuseIntensity)){
		// TODO handle
	}

	setDesc(desc);
}

void PointLight::serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst, void* opaque){
	ASceneActor::serialize(assets, dst, opaque);

	const Desc& desc = getDesc();

	dst.Set("attenConstat", desc.attenuation.constant);

	dst.Set("attenLinear", desc.attenuation.linear);

	dst.Set("attenExponential", desc.attenuation.quadratic);

	LuaObject luaColor = getLuaState()->newTable();
	lua::luaConv(desc.color, luaColor);
	dst.Set("color", luaColor);

	dst.Set("ambientIntensity", desc.ambientIntensity);

	dst.Set("diffuseIntensity", desc.diffuseIntensity);
}

} // </wt>