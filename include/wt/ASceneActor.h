#ifndef WT_ASCENEACTOR_H
#define WT_ASCENEACTOR_H


#include "wt/stdafx.h"


#include "wt/Transform.h"
#include "wt/Model.h"
#include "wt/AResourceSystem.h"

namespace wt{

class PhysicsActor;
class Scene;

class ASceneActor{
public:
	enum ActorType{
		eTYPE_TERRAIN,
		eTYPE_PARTICLE_EFFECT,
		eTYPE_MODELLED
	};

	struct AttachPoint{
		const ASceneActor* actor;
		String pointId;

		AttachPoint() : actor(NULL), pointId(""){
		}
	}; // </AttachPoint>

protected:
	/** Owner of the actor (scene which created it) */
	Scene* mParent;

	/** Name of the actor, might not be set and is not a unique identifier */
	String mName;
	
	/** Unique actor identification generated by the Scene */
	uint32_t mId;

	/** Indicates wheter or not he user data has been set by the user */
	bool mUserDataSet;

	/** Data set by the user */
	void* mUserData;

	/** Type of the actor, used for RTTI */
	ActorType mType;

	math::Transform mTransform;

	/** Scene actor may be paired with a physics actor for its physicall representation */
	PhysicsActor* mPhysicsActor;

	PhysicsActor* mBBox;

	AttachPoint mAttachPoint;

	/** Color of the bounding box for this object (used mainly for debugging) */
	Color mBoundingBoxColor;

public:
	ASceneActor(Scene* parent, ActorType type, uint32_t id, const String& name="");

	void setBoundingBoxColor(const Color& color);

	const Color& getBoundingBoxColor() const;

	virtual ~ASceneActor();

	void setBBox(PhysicsActor* box);

	/**
	 * Updates the bounding box physics actor with the new dimensons
	 *
	 * @param bounds - New bounding box dimensions (halfX, halfY, haflZ)
	 */
	void setBounds(const glm::vec3& bounds);

	physx::PxBounds3 getBounds() const;

	const AttachPoint& getAttachPoint() const;

	ActorType getActorType() const;

	void* getUserData() const;

	virtual bool getAttachPointTransform(const String& /*pointId*/, math::Transform& /*res*/) const;

	virtual bool validAttachPoint(const String& /*pointId*/) const;

	bool attach(const ASceneActor* actor, const String& pointId);

	void setUserData(void* data);

	PhysicsActor* getPhysicsActor();

	void setPhysicsActor(PhysicsActor* actor);

	bool hasUserData() const;

	uint32_t getId() const;

	const String& getName() const;

	math::Transform& getTransform();

	const math::Transform& getTransform() const;

	virtual void update(float /*dt*/);

	virtual void serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst, void* opaque=NULL);

	virtual void deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src, void* opaque=NULL);

}; // </ASceneActor>


}; // </wt>

#endif // </WT_ASCENEACTOR_H>