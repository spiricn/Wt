#ifndef WT_SPOTLIGHT_H
#define WT_SPOTLIGHT_H

#include "wt/ALight.h"

namespace wt
{

class SpotLight : public ALight{
friend class Scene;

public:
	struct Desc : public ALight::Desc{
		glm::vec3 position;
		glm::vec3 direction;
		float cutoff;

		struct{
			float constant;
			float linear;
			float quadratic;
		} attenuation;

		void deserialize(lua::State* luaState, const LuaPlus::LuaObject& src);

		void serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const;

		Desc();
	}; // </Desc>

	const Desc& getDesc() const;

protected:
	SpotLight(Scene* parent, uint32_t id);

	Desc& getDesc();

private:
	SpotLight(const SpotLight&);

	Desc mDesc;
}; // </SpotLight>

} // </wt>

#endif // </WT_SPOTLIGHT_H>