#ifndef WT_DIRECTIONALLIGHT_H
#define WT_DIRECTIONALLIGHT_H

#include "wt/ALight.h"

namespace wt
{

class DirectionalLight : public ALight{
friend class Scene;

public:
	struct Desc : public ALight::Desc{
		glm::vec3 direction;

		void deserialize(lua::State* luaState, const LuaPlus::LuaObject& src);

		void serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const;

		Desc();
	}; // </Desc>

	const Desc& getDesc() const;

protected:
	DirectionalLight(Scene* parent, uint32_t id);

	Desc& getDesc();

private:
	DirectionalLight(const DirectionalLight&);

	DirectionalLight::Desc mDesc;
}; // </DirectionalLight>

} // </wt>

#endif // </WT_DIRECTIONALLIGHT_H>