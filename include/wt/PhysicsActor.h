#ifndef WT_PHYSICSACTOR_H
#define WT_PHYSICSACTOR_H

using namespace physx;

#include "wt/Lua.h"

namespace wt{

class ASceneActor;
class Physics;

class PhysicsActor{
friend class Physics;

public:
	enum ActorType{
		eACTOR_TYPE_NONE,
		eSTATIC_ACTOR,
		eDYNAMIC_ACTOR,
		eREGION,
		eACTOR_TYPE_BBOX
	};

	enum ControlMode{
		eCTRL_MODE_NONE,
		ePHYSICS_MODE,
		eCONTROLLER_MODE
	};

	enum GeometryType{
		eGEO_TYPE_NONE,
		eBOX_GEOMETRY,
		eSPHERE_GEOMETRY,
		eHEIGHTMAP_GEOMETRY,
		eMESH_GEOMETRY
	};

	enum ControllerGeometry{
		eCTRL_TYPE_NONE,
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

		Desc() : type(eACTOR_TYPE_NONE), geometryType(eGEO_TYPE_NONE), controlMode(eCTRL_MODE_NONE), collisionMask(0xFFFFFFFF), group(0x01){
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



		void serialize(LuaPlus::LuaObject& table){
			// Actor type
			table.Set("type", type == eSTATIC_ACTOR ? "static" : "dynamic");

			// Controll
			table.Set("group", group);
			table.Set("collision_mask", collisionMask);


			// Physics-controlled
			if(controlMode == ePHYSICS_MODE){
				table.Set("ctrl.mode", "physics");

				// Box
				if(geometryType == eBOX_GEOMETRY){
					table.Set("geo.shape", "box");
					table.Set("geo.hx", geometryDesc.boxGeometry.hx);
					table.Set("geo.hy", geometryDesc.boxGeometry.hy);
					table.Set("geo.hz", geometryDesc.boxGeometry.hz);
				}
				// Sphere
				else if(geometryType == eSPHERE_GEOMETRY){
					table.Set("geo.shape", "sphere");
					table.Set("geo.rad", geometryDesc.sphereGeometry.radius);
				}
				else if(geometryType == eHEIGHTMAP_GEOMETRY){
					table.Set("geo.shape", "heightmap");
				}
				else if(geometryType == eMESH_GEOMETRY){
					table.Set("geo.shape", "mesh");
				}
				else{
					WT_THROW("TODO");
				}
			}
			// Controller-controlled
			else if(controlMode == eCONTROLLER_MODE){
				table.Set("ctrl.mode", "controller");

				if(controllerDesc.geometryType == eCAPSULE_CONTROLLER){
					table.Set("ctrl.shape", "capsule");
					table.Set("ctrl.rad", controllerDesc.geometryDesc.capsuleController.radius);
					table.Set("ctrl.height", controllerDesc.geometryDesc.capsuleController.height);
				}
				else if(controllerDesc.geometryType == eBOX_CONTROLLER){
					table.Set("ctrl.shape", "box");
					table.Set("ctrl.hx", controllerDesc.geometryDesc.boxController.hx);
					table.Set("ctrl.hy", controllerDesc.geometryDesc.boxController.hy);
					table.Set("ctrl.hz", controllerDesc.geometryDesc.boxController.hz);
				}
				else{
					WT_THROW("TODO");
				}
			}
			else{
				WT_THROW("TODO");
			}
		}


		void deserialize(const LuaPlus::LuaObject& table){
			String type, ctrlMode;
			lua::luaConv(table.Get("type"), type);
			lua::luaConv(table.Get("ctrl.mode"), ctrlMode);

			if(!type.compare("static")){
				this->type = eSTATIC_ACTOR;
			}
			else if(!type.compare("dynamic")){
				this->type = eDYNAMIC_ACTOR;
			}
			else{
				WT_THROW("TODO");
			}

			// Physics-controlled
			if(!ctrlMode.compare("physics")){
				controlMode = ePHYSICS_MODE;

				String shape;
				lua::luaConv(table.Get("geo.shape"), shape);

				if(!shape.compare("box")){
					this->geometryType = eBOX_GEOMETRY;

					lua::luaConv(table.Get("geo.hx"), geometryDesc.boxGeometry.hx);
					lua::luaConv(table.Get("geo.hy"), geometryDesc.boxGeometry.hy);
					lua::luaConv(table.Get("geo.hz"), geometryDesc.boxGeometry.hz);
				}
				else if(!shape.compare("sphere")){
					this->geometryType = eSPHERE_GEOMETRY;

					lua::luaConv(table.Get("geo.rad"), geometryDesc.sphereGeometry.radius);
				}
				else if(!shape.compare("mesh")){
					this->geometryType = eMESH_GEOMETRY;
				}
				else if(!shape.compare("heightmap")){
					this->geometryType = eHEIGHTMAP_GEOMETRY;
				}
				else{
					WT_THROW("TODO");
				}
			}
			// Controller-controlled
			else if(!ctrlMode.compare("controller")){
				controlMode = eCONTROLLER_MODE;

				String shape;
				lua::luaConv(table.Get("ctrl.shape"), shape);

				if(!shape.compare("capsule")){
					this->controllerDesc.geometryType = eCAPSULE_CONTROLLER;
					lua::luaConv(table.Get("ctrl.rad"), controllerDesc.geometryDesc.capsuleController.radius);
					lua::luaConv(table.Get("ctrl.height"), controllerDesc.geometryDesc.capsuleController.height);
				}
				else if(!shape.compare("box")){
					this->controllerDesc.geometryType = eBOX_CONTROLLER;
					lua::luaConv(table.Get("ctrl.hx"), controllerDesc.geometryDesc.boxController.hx);
					lua::luaConv(table.Get("ctrl.hy"), controllerDesc.geometryDesc.boxController.hy);
					lua::luaConv(table.Get("ctrl.hz"), controllerDesc.geometryDesc.boxController.hz);
				}
				else{
					WT_THROW("TODO");
				}
			}
			else{
				WT_THROW("TODO");
			}
		}

		bool isValid() const{
			if(geometryType == eMESH_GEOMETRY){
				return geometryDesc.meshGeometry.model != NULL;
			}
			else{
				return true;
			}
		}
	};

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

protected:
	void setController(PxController* ctrl){
		mPxController = ctrl;
	}

	void setDesc(const Desc& desc){
		mDesc = desc;
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

	ASceneActor* getSceneActor() const{
		return mSceneActor;
	}

	uint32_t getId() const{
		return mId;
	}

	const Desc& getDesc() const{
		return mDesc;
	}

	PhysicsActor(uint32_t id, const String& name, ActorType type, ControlMode mode, PxActor* actor,
		PxController* controller, ASceneActor* sceneActor) : mActorType(type), mControlMode(mode),
		mPxActor(actor), mPxController(controller), mSceneActor(sceneActor), mName(name), mId(id){
	}

}; // </PhysicsActor>

}; // </wt>

#endif // </WT_PHYSICSACTOR_H>