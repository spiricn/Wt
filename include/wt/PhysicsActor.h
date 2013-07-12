#ifndef WT_PHYSICSACTOR_H
#define WT_PHYSICSACTOR_H

using namespace physx;

namespace wt{

class Physics;

class PhysicsActor{
friend class Physics;

public:
	enum ActorType{
		eSTATIC_ACTOR,
		eDYNAMIC_ACTOR,
		eREGION
	};

	enum ControlMode{
		ePHYSICS_MODE,
		eCONTROLLER_MODE
	};

	enum GeometryType{
		eBOX_GEOMETRY,
		eSPHERE_GEOMETRY,
		eHEIGHTMAP_GEOMETRY,
		eMESH_GEOMETRY
	};

	enum ControllerGeometry{
		eBOX_CONTROLLER,
		eCAPSULE_CONTROLLER
	};

	struct Desc{

		ActorType type;
		GeometryType geometryType;
		math::Transform pose;
		uint32_t group;
		uint32_t collisionMask;

		ControlMode controlMode;


		Desc() : collisionMask(0xFFFFFFFF), group(0x01){
		}

		bool isValid() const{
			if(geometryType == eMESH_GEOMETRY){
				return geometryDesc.meshGeometry.model != NULL;
			}
			else{
				return true;
			}
		}

		
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
	};

private:
	ActorType mActorType;
	ControlMode mControlMode;
	PxActor* mPxActor;
	PxController* mPxController;
	SceneActor* mSceneActor;
	String mName;
	void* mUserData;
	uint32_t mId;

protected:
	void setController(PxController* ctrl){
		mPxController = ctrl;
	}

public:
	ControlMode getControlMode() const{
		return mControlMode;
	}

	bool isControlled() const{
		return mControlMode == eCONTROLLER_MODE;
	}

	PxController* getController(){
		return mPxController;
	}


	PxActor* getPxActor(){
		return mPxActor;
	}

	const String& getName() const{
		return mName;
	}

	ActorType getType() const{
		return mActorType;
	}

	SceneActor* getSceneActor() const{
		return mSceneActor;
	}

	uint32_t getId() const{
		return mId;
	}

	PhysicsActor(uint32_t id, const String& name, ActorType type, ControlMode mode, PxActor* actor,
		PxController* controller, SceneActor* sceneActor) : mActorType(type), mControlMode(mode),
		mPxActor(actor), mPxController(controller), mSceneActor(sceneActor), mName(name), mId(id){
	}

}; // </PhysicsActor>

}; // </wt>

#endif // </WT_PHYSICSACTOR_H>