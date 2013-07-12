#ifndef WT_LIGHT_H
#define WT_LIGHT_H

#include "wt/ASerializable.h"

namespace wt{

class ALight : public Lua::ASerializable{
public:
	bool mActive;
	Color mColor;
	float mAmbientIntesity;
	float mDiffuseItensity;

	ALight(const Color& color, float ambientIntesity, float diffuseItensity) : 
		mColor(color), mAmbientIntesity(ambientIntesity), mDiffuseItensity(diffuseItensity), mActive(true){
	}

	virtual ~ALight(){
	}

	virtual void serialize(LuaPlus::LuaObject& dst){
		LuaObject luaColor;
		LUA_NEW_TABLE(luaColor);

		Lua::luaConv(mColor, luaColor);
		dst.Set("color", luaColor);

		dst.Set("ambientIntensity", mAmbientIntesity);

		dst.Set("diffuseIntensity", mDiffuseItensity);
	}

	virtual void deserialize(const LuaPlus::LuaObject& src){
		WT_ASSERT(src.IsTable()
			&& src.Get("ambientIntensity").IsNumber()
			&& src.Get("diffuseIntensity").IsNumber()
			&& src.Get("color").IsTable(),
			"Invalid light lua table");

		if(!Lua::luaConv(src.Get("color"), mColor)){
			// TODO handle
		}

		if(!Lua::luaConv(src.Get("ambientIntensity"), mAmbientIntesity)){
			// TODO handle
		}

		if(!Lua::luaConv(src.Get("diffuseIntensity"), mDiffuseItensity)){
			// TODO handle
		}
	}

}; // </ALight>

class DirectionalLight : public ALight{
public:
	glm::vec3 mDirection;

	DirectionalLight(const Color& color=Color::white(), float ambientIntesity=1.0f,
		float diffuseItensity=1.0f, const glm::vec3& dir=glm::vec3(0,-1,0)) : ALight(color, ambientIntesity, diffuseItensity),
		mDirection(dir){
	}

	void deserialize(const LuaPlus::LuaObject& src){
		ALight::deserialize(src);

		if(!Lua::luaConv(src.Get("direction"), mDirection)){
			// TODO handle
		}	
	}

	void serialize(LuaPlus::LuaObject& dst){
		ALight::serialize(dst);

		LuaObject luaDir;
		LUA_NEW_TABLE(luaDir);

		Lua::luaConv(mDirection, luaDir);
		dst.Set("direction", luaDir);
	}
}; // </DirectionalLight>

class PointLight : public ALight{
public:
	glm::vec3 mPosition;
	struct{
		float constant;
		float linear;
		float exponential;
	} mAttenuation;

	PointLight (const Color& color=Color::white(), float ambientIntesity=1.0f,
		float diffuseItensity=1.0f, const glm::vec3& pos=glm::vec3(0, 0, 0)) : ALight(color, ambientIntesity, diffuseItensity),
		mPosition(pos){
			mAttenuation.constant = 1.0f;
			mAttenuation.linear = 0.1f;
			mAttenuation.exponential = 0.0f;
	}

	void deserialize(const LuaPlus::LuaObject& src){
		ALight::deserialize(src);
		if(!Lua::luaConv(src.Get("position"), mPosition)){
			// TODO handle
		}

		Lua::luaConv(src.Get("attenConstat"), mAttenuation.constant);

		Lua::luaConv(src.Get("attenLinear"), mAttenuation.linear);

		Lua::luaConv(src.Get("attenExponential"), mAttenuation.exponential);
	}

	void serialize(LuaPlus::LuaObject& dst){
		ALight::serialize(dst);

		LuaObject luaPos;
		LUA_NEW_TABLE(luaPos);

		Lua::luaConv(mPosition, luaPos);
		dst.Set("position", luaPos);

		dst.Set("attenConstat", mAttenuation.constant);

		dst.Set("attenLinear", mAttenuation.linear);

		dst.Set("attenExponential", mAttenuation.exponential);
	}

}; // </PointLight>


class SpotLight : public PointLight{
public:
	glm::vec3 direction;
	float cutoff;

	SpotLight(const Color& color=Color::white(), float ambientIntesity=1.0f,
		float diffuseItensity=1.0f, const glm::vec3& pos=glm::vec3(0, 0, 0)) : PointLight(color, ambientIntesity, diffuseItensity, pos){
	}
}; // </SpotLight>

}; // </wt>

#endif
