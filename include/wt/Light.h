#ifndef WT_LIGHT_H
#define WT_LIGHT_H

#include "wt/ASerializable.h"

namespace wt{

class ALight : public lua::ASerializable{
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

	virtual void serialize(lua::State* luaState, LuaPlus::LuaObject& dst){
		LuaObject luaColor = luaState->newTable();

		lua::luaConv(mColor, luaColor);
		dst.Set("color", luaColor);

		dst.Set("ambientIntensity", mAmbientIntesity);

		dst.Set("diffuseIntensity", mDiffuseItensity);
	}

	virtual void deserialize(lua::State* luaState, const LuaPlus::LuaObject& src){
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

}; // </ALight>

class DirectionalLight : public ALight{
public:
	glm::vec3 mDirection;

	DirectionalLight(const Color& color=Color::white(), float ambientIntesity=1.0f,
		float diffuseItensity=1.0f, const glm::vec3& dir=glm::vec3(0,-1,0)) : ALight(color, ambientIntesity, diffuseItensity),
		mDirection(dir){
	}

	void deserialize(lua::State* luaState, const LuaPlus::LuaObject& src){
		ALight::deserialize(luaState, src);

		if(!lua::luaConv(src.Get("direction"), mDirection)){
			// TODO handle
		}	
	}

	void serialize(lua::State* luaState, LuaPlus::LuaObject& dst){
		ALight::serialize(luaState, dst);

		LuaObject luaDir = luaState->newTable();

		lua::luaConv(mDirection, luaDir);
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

	void deserialize(lua::State* luaState, const LuaPlus::LuaObject& src){
		ALight::deserialize(luaState, src);
		if(!lua::luaConv(src.Get("position"), mPosition)){
			// TODO handle
		}

		lua::luaConv(src.Get("attenConstat"), mAttenuation.constant);

		lua::luaConv(src.Get("attenLinear"), mAttenuation.linear);

		lua::luaConv(src.Get("attenExponential"), mAttenuation.exponential);
	}

	void serialize(lua::State* luaState, LuaPlus::LuaObject& dst){
		ALight::serialize(luaState, dst);

		LuaObject luaPos = luaState->newTable();

		lua::luaConv(mPosition, luaPos);
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
