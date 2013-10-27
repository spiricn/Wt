#ifndef WT_PHYSICSACTOR_H
#define WT_PHYSICSACTOR_H

using namespace physx;

#include "wt/Lua.h"
#include "wt/ATransformable.h"
#include "wt/Model.h"

namespace wt{

class ASceneActor;
class Physics;

class PhysicsActor : public ATransformable{
friend class Physics;
public:

	enum ActorType{
		eTYPE_INVALID = -1,
		eTYPE_STATIC = 0,
		eTYPE_DYNAMIC,
		eTYPE_REGION,
		eTYPE_BOUNDING_BOX
	}; // </ActorType>

	enum ControlMode{
		eCTRL_MODE_INVALID = -1,
		eCTRL_MODE_NONE = 0,
		eCTRL_MODE_PHYSICS,
		eCTRL_MODE_CONTROLLER
	}; // </ControlMode>

	enum GeometryType{
		eGEOMETRY_INVALID = -1,
		eGEOMETRY_PLANE = 0,
		eGEOMETRY_BOX,
		eGEOMETRY_SPHERE,
		eGEOMETRY_HEIGHTMAP,
		eGEOMETRY_MESH
	}; // </GeometryType>

	enum ControllerGeometry{
		eCTRL_GEOMETRY_INVALID = -1,
		eCTRL_GEOMETRY_BOX,
		eCTRL_GEOMETRY_CAPSULE,
	};

	struct Desc{
		ActorType type;
		GeometryType geometryType;
		glm::mat4 pose;
		uint32_t group;
		uint32_t collisionMask;
		ControlMode controlMode;

		Desc();

		union GeometryDesc{
			struct BoxGeometry{
				float hx, hy, hz;
			} boxGeometry;

			struct SphereGeometry{
				float radius;
			} sphereGeometry;

			struct HeightfieldGeometry{
				const Buffer<int16_t>* heightmap;
				uint32_t numRows;
				uint32_t numCols;
				float heightScale;
				float rowScale;
				float colScale;
			} heightfieldGeometry;

			struct MeshGeometry{
				Model* model;
			} meshGeometry;

		} geometryDesc;

		struct ControllerDesc{
			ControllerGeometry geometryType;

			union GeometryDesc{
				struct BoxController{
					float hx, hy, hz;
				} boxController;

				struct CapsuleController{
					float radius;
					float height;
				} capsuleController;

			} geometryDesc;

		} controllerDesc;

		void serialize(LuaPlus::LuaObject& table);

		void deserialize(const LuaPlus::LuaObject& table);

		bool isValid() const;

	}; // </Desc>

public:
	PhysicsActor(uint32_t id, const String& name, ActorType type, ControlMode mode, PxActor* actor,
		PxController* controller, ASceneActor* sceneActor);

	ControlMode getControlMode() const;

	bool isControlled() const;

	PxController* getController();

	PxActor* getPxActor();

	const String& getName() const;

	ActorType getType() const;

	ASceneActor* getSceneActor() const;

	uint32_t getId() const;

	const Desc& getDesc() const;

	void setTranslation(const glm::vec3& translation);

	void setRotation(const glm::quat& rotation);

	void setScale(const glm::vec3& scale);

	void getScale(glm::vec3& result) const;

	void getTranslation(glm::vec3& result) const;

	void getRotation(glm::quat& result) const;

protected:
	void setController(PxController* ctrl);

	void setDesc(const Desc& desc);

private:
	ActorType mActorType;
	ControlMode mControlMode;
	PxActor* mPxActor;
	PxController* mPxController;
	ASceneActor* mSceneActor;
	String mName;
	void* mUserData;
	uint32_t mId;
	Desc mDesc;
}; // </PhysicsActor>

}; // </wt>

#endif // </WT_PHYSICSACTOR_H>