#ifndef WT_POINTLIGHT_H
#define WT_POINTLIGHT_H

#include "wt/ALight.h"

namespace wt
{

class PointLight : public ALight{
friend class Scene;

public:
	struct Desc : public ALight::Desc{
		glm::vec3 position;

		struct{
			float constant;
			float linear;
			float quadratic;
		} attenuation;

		void deserialize(lua::State* luaState, const LuaPlus::LuaObject& src);

		void serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const;

		float calculateBoundingSphere() const;

		Desc();
	}; // </Desc>

	const Desc& getDesc() const;

	void setDesc(const Desc& desc) const;

protected:
	PointLight(Scene* parent, uint32_t id);

	Desc& getDesc();



private:
	PointLight(const PointLight&);

	Desc mDesc;
}; // </PointLight>

} // </wt>

#endif // </WT_POINTLIGHT_H>