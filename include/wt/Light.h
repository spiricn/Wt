#ifndef WT_LIGHT_H
#define WT_LIGHT_H

#include "wt/ASerializable.h"
#include "wt/Color.h"
#include "wt/lua/State.h"

namespace wt{

class ALight : public lua::ASerializable{
public:
	bool mActive;
	Color mColor;
	float mAmbientIntesity;
	float mDiffuseItensity;

	ALight(const Color& color, float ambientIntesity, float diffuseItensity);

	virtual ~ALight();

	virtual void serialize(lua::State* luaState, LuaPlus::LuaObject& dst);

	virtual void deserialize(lua::State* luaState, const LuaPlus::LuaObject& src);
}; // </ALight>

class DirectionalLight : public ALight{
public:
	glm::vec3 mDirection;

	DirectionalLight(const Color& color=Color::white(), float ambientIntesity=1.0f,
		float diffuseItensity=1.0f, const glm::vec3& dir=glm::vec3(0,-1,0));

	void deserialize(lua::State* luaState, const LuaPlus::LuaObject& src);

	void serialize(lua::State* luaState, LuaPlus::LuaObject& dst);
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
		float diffuseItensity=1.0f, const glm::vec3& pos=glm::vec3(0, 0, 0));

	void deserialize(lua::State* luaState, const LuaPlus::LuaObject& src);

	void serialize(lua::State* luaState, LuaPlus::LuaObject& dst);

	float calculateBoundingSphere();
}; // </PointLight>


class SpotLight : public PointLight{
public:
	glm::vec3 direction;
	float cutoff;

	SpotLight(const Color& color=Color::white(), float ambientIntesity=1.0f,
		float diffuseItensity=1.0f, const glm::vec3& pos=glm::vec3(0, 0, 0));
}; // </SpotLight>

}; // </wt>

#endif
