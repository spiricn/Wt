#ifndef WT_COLLIDERACTOR_H
#define WT_COLLIDERACTOR_H

#include "wt/ASceneActor.h"
#include "wt/PhysicsActor.h"
#include "wt/Transform.h"

namespace wt
{

class ColliderActor : public ASceneActor, public ATransformable{
public:
	struct DeserializationData{
		PhysicsActor::Desc pxDesc;
		bool phyiscs;
	}; // </DeserializationData>

public:
	ColliderActor(Scene* parent, uint32_t id, const String& name="");

	~ColliderActor();

	ATransformable* getTransformable() const;

	void setTranslation(const glm::vec3& translation);

	void setRotation(const glm::quat& rotation);

	void setScale(const glm::vec3& scale);

	void getScale(glm::vec3& result) const;

	void getTranslation(glm::vec3& result) const;

	void getRotation(glm::quat& result) const;
	
	void serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst, void* opaque=NULL);

	void deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src, void* opaque=NULL);
	
	void physicsControl(const glm::vec3& translation);

	void physicsControl(const glm::quat& rotation);

private:
	math::Transform mTransform;
}; // </ColliderActor>

} // </wt>

#endif // </WT_COLLIDERACTOR_H>
