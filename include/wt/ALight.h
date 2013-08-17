#ifndef WT_LIGHT_H
#define WT_LIGHT_H

#include "wt/ASerializable.h"
#include "wt/Color.h"
#include "wt/lua/State.h"

namespace wt{

class Scene;

class ALight{
friend class Scene;

public:
	enum Type{	
		eTYPE_INVALID = -1,
		eTYPE_DIRECTIONAL = 0,
		eTYPE_POINT,
		eTYPE_SPOT,
	}; // </LightType>

	struct Desc : public lua::ASerializable{
		bool enabled;
		Color color;
		float ambientIntensity;
		float diffuseIntensity;

		virtual ~Desc();

		Desc();

		virtual void serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const;

		virtual void deserialize(lua::State* luaState, const LuaPlus::LuaObject& src);
	}; // </Settings>

	virtual ~ALight();

	Scene* getParent() const;

	uint32_t getId() const;

	Type getType() const;

protected:
	ALight(Scene* parent, uint32_t id, Type type);

private:
	Scene* mParent;
	uint32_t mId;
	Type mType;

}; // </ALight>

}; // </wt>

#endif
