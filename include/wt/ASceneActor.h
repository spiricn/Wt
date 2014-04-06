#ifndef WT_ASCENEACTOR_H
#define WT_ASCENEACTOR_H

#include "wt/stdafx.h"

#include "wt/Model.h"
#include "wt/AResourceSystem.h"

namespace wt{

namespace lua
{
	class State;
};

class PhysicsActor;
class Scene;

class ASceneActor{
friend class Scene;

public:
	enum ActorType{
		eTYPE_INVALID = -1,
		eTYPE_TERRAIN = 0,
		eTYPE_PARTICLE_EFFECT,
		eTYPE_MODELLED,
		eTYPE_POINT_LIGHT,
		eTYPE_SOUND,
		eTYPE_COLLIDER,
		eTYPE_CUSTOM,
		eTYPE_MAX
	}; // </ActorType>

	struct AttachPoint{
		ASceneActor* actor;
		String pointId;

		AttachPoint() : actor(NULL), pointId(""){
		}
	}; // </AttachPoint>

	struct AttachDesc{
		ASceneActor* src;
		ASceneActor* dst;
		String pointId;

		AttachDesc() : src(NULL), dst(NULL){
		}
	}; // </AttachDesc>

	struct Desc{
	}; // </Desc>

	void detach();

	lua::State* getLuaState();

	virtual ATransformable* getTransformable() const = 0;

	bool isVisible() const;

	void setVisible(bool state);

	virtual void physicsControl(const glm::vec3& translation);

	virtual void physicsControl(const glm::quat& rotation);

	ASceneActor(Scene* parent, ActorType type, uint32_t id, const String& name="");

	void setBoundingBoxColor(const Color& color);

	const Color& getBoundingBoxColor() const;

	virtual ~ASceneActor();

	void setBBox(PhysicsActor* box);

	Scene* getScene() const;

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

	virtual bool getAttachPointTransform(const String& pointId, glm::mat4& res) const;

	virtual bool validAttachPoint(const String& pointId) const;

	bool attach(ASceneActor* actor, const String& pointId);

	void setUserData(void* data);

	PhysicsActor* getPhysicsActor() const;

	void setPhysicsActor(PhysicsActor* actor);

	bool hasUserData() const;

	uint32_t getId() const;

	const String& getName() const;

	void setName(const String& name);

	virtual void update(float /*dt*/);

	virtual void serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst, void* opaque=NULL);

	virtual void deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src, void* opaque=NULL);

	virtual void destroy();

private:

	void setId(uint32_t id);

	void setParent(Scene* scene);

	typedef std::vector<ASceneActor*> ActorList;
	ActorList mAttachedActors;

	lua::State* mLuaState;

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

	/** Scene actor may be paired with a physics actor for its physicall representation */
	PhysicsActor* mPhysicsActor;

	PhysicsActor* mBBox;

	AttachPoint mAttachPoint;

	/** Color of the bounding box for this object (used mainly for debugging) */
	Color mBoundingBoxColor;

	void setLuaState(lua::State* state);

	bool mVisible;
}; // </ASceneActor>


}; // </wt>

#endif // </WT_ASCENEACTOR_H>