#include "wt/stdafx.h"

#include "wt/Physics.h"
#include "wt/camera.h"
#include "Wt/frustum.h"

#define TD_TRACE_TAG "Physics"

namespace wt{

enum InternalGroups{
	eIG_HEIGHTMAP = (1 << 0),
	eIG_REGION = (1 << 1),
	eIG_BBOX = (1 << 2)
}; // </EngineGroups>

const EvtType RaycastHitEvent::TYPE = "RaycastHit";
const EvtType RegionEvent::TYPE = "RegionEvent";

class SimulationCalblack : public PxSimulationEventCallback{
private:
	EventManager* mEventManager;

public:
	SimulationCalblack(EventManager* eventManager) : mEventManager(eventManager){
	}

	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs){
		LOG("onContact");

		if(pairHeader.flags & PxContactPairHeaderFlag::eDELETED_ACTOR_0 || 
					pairHeader.flags & PxContactPairHeaderFlag::eDELETED_ACTOR_1){
			// One of the actors was removed from the scene
			LOG("Actor deleted, ignoring contact...");
			return;
		}

		for(int i=0; i<nbPairs; i++){
			const PxContactPair& pair = pairs[i];

			if(pair.events & PxPairFlag::eNOTIFY_TOUCH_FOUND || pair.events & PxPairFlag::eNOTIFY_TOUCH_LOST){
				
				PhysicsActor* actor1 = static_cast<PhysicsActor*>( pair.shapes[0]->getActor().userData );
				PhysicsActor* actor2 = static_cast<PhysicsActor*>( pair.shapes[1]->getActor().userData );

				LOG("TOUCH FOUND! %d %d", actor1->getType(), actor2->getType());

				RegionEvent* evt = new RegionEvent(
					actor1->getType() == PhysicsActor::eREGION ? actor2 : actor1, 
					actor1->getType() == PhysicsActor::eREGION ? actor1->getId() : actor2->getId(),
					pair.events & PxPairFlag::eNOTIFY_TOUCH_FOUND ? RegionEvent::eACTOR_ENTERED_REGION : RegionEvent::eACTOR_LEFT_REGION
				);

				mEventManager->queueEvent(evt);
			}
			else{
				LOG("WTF?");
			}
		}
	}

	void onTrigger(PxTriggerPair* pairs, PxU32 count){
		LOG("onTrigger");
	}

	void onConstraintBreak(PxConstraintInfo*, PxU32){
	}

	void onWake(PxActor** , PxU32 ){
	}

	void onSleep(PxActor** , PxU32 ){
	}
};



Physics::Physics(EventManager* eventManager) : mTimeAccumulator(1/60.0f){
    LOGV("Initializing Physx");

	// Setup events
	mEventManager = eventManager;

	mEventEmitter.hook(mEventManager,
		2,
		RegionEvent::TYPE,
		RaycastHitEvent::TYPE
	);

    // Initialize the SDK
    mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocator, mDefaultErrorCallback);
    if(mFoundation == NULL){
        WT_THROW("Error creating PxFoundation");
    }

    mSdk = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale());
    if(mSdk == NULL){
        WT_THROW("Error creating PxSdk");
    }

    // Extensions
    if(!PxInitExtensions(*mSdk)){
        WT_THROW("Error initializing PxExtensions");
    }

    // Dispatcher
    mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
    if(mCpuDispatcher == NULL){
        WT_THROW("Error creating PxCpuDispatcher");
    }

    // Cooking
    mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams());
		
    if(mCooking == NULL){
        WT_THROW("Error creating cooking interface");
    }
        
    // Filter shader
    mDefaultFilterShader = filterShader;

    // Character controler
    mCtrlManager = PxCreateControllerManager(*mFoundation);
    if(mCtrlManager == NULL){
        WT_THROW("Error creating PxCharacterController");
    }

    // Scene
    PxSceneDesc sceneDesc(mSdk->getTolerancesScale());
    sceneDesc.filterShader = mDefaultFilterShader;
    sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
    sceneDesc.cpuDispatcher = mCpuDispatcher;
    sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVETRANSFORMS; // | PxSceneFlag::eENABLE_KINEMATIC_PAIRS | PxSceneFlag::eENABLE_KINEMATIC_STATIC_PAIRS ;
	sceneDesc.simulationEventCallback = new SimulationCalblack(mEventManager);;

    if(!sceneDesc.isValid()){
        WT_THROW("Invalid Px scene description");
    }		

    mScene = mSdk->createScene(sceneDesc);
    if(mScene == NULL){
        WT_THROW("Error creating PxScene");
    }

    // material
    mDefaultMaterial = mSdk->createMaterial(0.5, 0.5, 0.5);

    // ground plane
    PxTransform pose = PxTransform(PxVec3(0.0f, -100.0f, 0.0f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f))
        );
    PxRigidStatic* plane = mSdk->createRigidStatic(pose);
    PxShape* shape = plane->createShape(PxPlaneGeometry(), *mDefaultMaterial);
    mScene->addActor(*plane);

    LOGV("Physx successfuly initialized");
}

Physics::~Physics(){
	mCooking->release();
	mCtrlManager->release();
	mCpuDispatcher->release();
	mDefaultMaterial->release();

	PxCloseExtensions();
	mScene->release();
	mSdk->release();
		
	mFoundation->release();
}

PxFilterFlags Physics::filterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, 
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize){

	// let triggers through
	if(PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		LOG("Trigger");
		return PxFilterFlag::eDEFAULT;
	}

	/*
		Groups are stored in word0 of filter data structure:
			object A group = filterData0.word0
			object B group = filterData1.word0

		Information if a specific object collides with another object's group is stored in word1:
			object A collides with object B group ? filterData1.word0 & filterData0.word1
			object B collides with object A group ? filterData0.word0 & filterData1.word1

			Both of these conditions need to be satisfied in order for a collision to occur!
	*/

	// Is either one of these a region?
	if(filterData0.word3 & eIG_REGION || filterData1.word3 & eIG_REGION){
		LOG("REGION COLLISION!");
		// If so, don't allow collision but do allow callback notifications
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST | PxPairFlag::eNOTIFY_TOUCH_FOUND;
	}
	else if((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1)){
		// Collision occured
		pairFlags |= PxPairFlag::eCONTACT_DEFAULT /* allow collision, no need for callbacks yet*/;
	}
	
	return PxFilterFlag::eDEFAULT;
}


uint32_t Physics::createRegion(const String& name, const glm::vec3& position, float radius){
	PhysicsActor::Desc desc;

	desc.type = PhysicsActor::eREGION;
	desc.geometryType = PhysicsActor::eSPHERE_GEOMETRY;
	desc.geometryDesc.sphereGeometry.radius = radius;
	desc.pose.setPosition(position);

	PhysicsActor* actor = createActor(NULL, desc);

	return actor->getId();
}


void pxConvert(const physx::PxExtendedVec3& src, glm::vec3& dst){
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

void pxConvert(const glm::vec3& src, physx::PxVec3& dst){
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

void pxConvert(const glm::vec3& src, physx::PxExtendedVec3& dst){
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

void pxConvert(const PxTransform& src, math::Transform& dst){
	dst.setPosition( glm::vec3(src.p.x, src.p.y, src.p.z) );
    dst.setQuat( src.q.x, src.q.y, src.q.z, src.q.w );
}

void pxConvert(const math::Transform& src, PxTransform& dst){
	dst.p.x = src.getPosition().x;
	dst.p.y = src.getPosition().y;
	dst.p.z = src.getPosition().z;

	dst.q.x = src.getQuat().x;
	dst.q.y = src.getQuat().y;
	dst.q.z = src.getQuat().z;
	dst.q.w = src.getQuat().w;
}

void Physics::convertTransform(const PxTransform& src, math::Transform& dst){
    dst.setPosition( glm::vec3(src.p.x, src.p.y, src.p.z) );
    dst.setQuat( src.q.x, src.q.y, src.q.z, src.q.w );
}

void Physics::convertTransform(const math::Transform& src, PxTransform& dst){
	dst.p.x = src.getPosition().x;
	dst.p.y = src.getPosition().y;
	dst.p.z = src.getPosition().z;

	dst.q.x = src.getQuat().x;
	dst.q.y = src.getQuat().y;
	dst.q.z = src.getQuat().z;
	dst.q.w = src.getQuat().w;
}

bool Physics::connectToVisualDebugger(const String& addr, int32_t port, int32_t timeout){
	#ifdef PX_SUPPORT_VISUAL_DEBUGGER
	// Physx Visual Debugger
	if(mSdk->getPvdConnectionManager() == NULL){
		WT_THROW("Unable to get PVD connection manager");
	}
	else{
		LOGV("Connecting to PVD %s : %d ...", addr.c_str(), port);

		PVD::PvdConnection* theConnection = PxVisualDebuggerExt::createConnection(mSdk->getPvdConnectionManager(),
			addr.c_str(), port, timeout, PxVisualDebuggerExt::getAllConnectionFlags());
		if(theConnection){
			LOGV("Connected");
			return true;
		}
		else{
			return false;
		}
	}
	#else
	LOGW("Unable to connect to PVD (PX_SUPPORT_VISUAL_DEBUGGER compile flag must be set");
	#endif
}

void Physics::update(float dt){
#if 0
	if(!mTimeAccumulator.update(dt)){
		return;
	}
	mScene->simulate( mTimeAccumulator.getStep() );
#else
	mScene->simulate( dt);
#endif
	

    mScene->fetchResults(true);

    PxU32 numActiveTrans;
    PxActiveTransform* activeTransforms = mScene->getActiveTransforms(numActiveTrans);

    for(PxU32 i=0; i<numActiveTrans; i++){
        PxActor* pxActor = activeTransforms[i].actor;
        PhysicsActor* actor = static_cast<PhysicsActor*>( pxActor->userData );

		
        if(actor==NULL){
            LOGW("No physx actor found in PxActor object");
            continue;
        }

		if(!actor->getSceneActor()){
			continue;
		}


		if(actor->isControlled()){
			const PxExtendedVec3& pos = actor->getController()->getFootPosition();
			actor->getSceneActor()->getTransform().setPosition(pos.x, pos.y, pos.z);
		}
		else if(actor->getSceneActor()){
			convertTransform(activeTransforms[i].actor2World,
				actor->getSceneActor()->getTransform()
				);
		}
		else{
			LOGW("No scene actor associated with physx actor!");
		}
    }

	//// regions
	//static float regionTimer = 0.5f;
	//regionTimer -= dt;
	//if(regionTimer <= 0.0f){
	//	regionTimer = 0.5f;

	//	const PxU32 bufferSize = 256;
	//	PxShape* hitBuffer[bufferSize];

	//	for(RegionMap::iterator i=mRegions.begin(); i!=mRegions.end(); i++){
	//		PxI32 hitNum = mScene->overlapMultiple(i->second->getGeometry(), i->second->getPose(), hitBuffer, bufferSize);

	//		std::vector<PhysicsActor*> containedAtors;
	//		for(PxI32 j=0; j<hitNum; j++){
	//			PhysicsActor* actor = static_cast<PhysicsActor*>(hitBuffer[j]->getActor().userData);
	//			containedAtors.push_back(actor);

	//			if(!i->second->containsActor(actor)){
	//				i->second->addActor(actor);

	//				mEventManager->queueEvent(
	//					new ActorEnteredRegionEvent(actor, i->second)
	//				);
	//			}
	//		}

	//		for(ActorMap::iterator j=mActors.begin(); j!=mActors.end(); j++){

	//			if(std::find(containedAtors.begin(), containedAtors.end(), j->second) == containedAtors.end() && i->second->containsActor(j->second)){
	//				i->second->removeActor(j->second);
	//			
	//				mEventManager->queueEvent(
	//					new ActorLeftRegionEvent(j->second, i->second)
	//					);
	//			}
	//			else{
	//			}

	//			//if(i->second
	//		}
	//	}
	//}
}

void Physics::setTimeStep(float step){
	mTimeAccumulator.setStep(step);
}


Sp<Buffer<PhysicsActor*>> Physics::getActorsInRegion(const glm::vec3& pos, float radius, uint32_t groups){
		PxSphereGeometry sphere(radius);
		PxTransform pose = PxTransform::createIdentity();
		pxConvert(pos, pose.p);

		const PxU32 bufferSize = 256;
		PxShape* hitBuffer[bufferSize];

		PxSceneQueryFilterData filterData;
		filterData.data.setToDefault();
		filterData.data.word0 = groups;

		PxI32 hitNum = mScene->overlapMultiple(sphere, pose, hitBuffer, bufferSize, filterData);

		if(hitNum == -1){
			// TODO increase buffer size dynamically?
			LOGW("Buffer overflow (more than %d actors contained in radius)", bufferSize);
			return NULL;
		}
		else{
			Buffer<PhysicsActor*>* bfr = new Buffer<PhysicsActor*>();
			bfr->create(hitNum);
			for(int32_t i=0; i<hitNum; i++){
				if(hitBuffer[i]->getActor().userData){
					bfr->put(
						static_cast<PhysicsActor*>(hitBuffer[i]->getActor().userData)
						);
				}
				else{
					LOGW("No user data found for PxActor!");
				}
			}

			return bfr;
		}
	}

uint32_t Physics::generateActorId(){
	uint32_t id=mActors.size();
	while(true){
		if(getActorById(id) != NULL){
			++id;
		}
		else{
			break;
		}
	}
	return id;
}

PhysicsActor* Physics::getActorById(uint32_t id){
	ActorMap::iterator i = mActors.find(id);
	return i == mActors.end() ? NULL : i->second;
}

float Physics::getTerrainHeightAt(const glm::vec2& pos){
	PxSceneQueryFilterData filterData;
	filterData.data.setToDefault();
	filterData.data.word3 = eIG_HEIGHTMAP;

	PxRaycastHit hit;
	
	bool status = mScene->raycastSingle(PxVec3(pos.x, 1000.0f, pos.y),
		PxVec3(0.0f, -1.0f, 0.0f),
		PX_MAX_F32, PxSceneQueryFlag::eIMPACT|PxSceneQueryFlag::eDISTANCE, hit, filterData);

	if(status){
		return hit.impact.y;
	}
	else{
		return -PX_MAX_F32;
	}
}


PxMaterial& Physics::getDefaultMaterial(){
	return *mDefaultMaterial;
}

PxPhysics* Physics::getSdk(){
	return mSdk;
}

PxScene* Physics::getScene(){
	return mScene;
}

PxTriangleMesh* Physics::cook(PxBoundedData& vertices, PxBoundedData& indices){
	// Setup mesh description
    PxTriangleMeshDesc desc;

	desc.points = vertices;
	desc.triangles = indices;

    WT_ASSERT(desc.isValid(), "Invalid triangle mesh description");

    PxToolkit::MemoryOutputStream writeBuffer;
    if(!mCooking->cookTriangleMesh(desc, writeBuffer)){
        WT_THROW("Error cooking triangle mesh");
    }

    PxToolkit::MemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

    PxTriangleMesh* res = mSdk->createTriangleMesh(readBuffer);

    return res;
}

PxTriangleMesh* Physics::cook(Gl::Batch& src){
	WT_ASSERT(
		src.isGenerated(), "Can not cook ungenerated batch");
	PxBoundedData vertices;

	vertices.data = src.getVertexBuffer().map(Gl::Buffer::eREAD_ONLY);
    vertices.count = src.getNumVertices();
    vertices.stride = src.getVertexSize();

	PxBoundedData indices;

	indices.data = src.getIndexBuffer().map(Gl::Buffer::eREAD_ONLY);
    indices.count = src.getNumIndices()/3;
    indices.stride = 3*src.getIndexSize();

	PxTriangleMesh* res = cook(vertices, indices);

	src.unmapVertexBuffer();
    src.unmapIndexBuffer();

	return res;
}

void Physics::removeActor(PhysicsActor* actor){
	if(actor->isControlled()){
		actor->getController()->release();
	}
	else{
		mScene->removeActor(*actor->getPxActor());
	}
	mActors.erase(actor->getId());
}


bool Physics::pick(const glm::vec3& origin, const glm::vec3& direction, RaycastHitEvent& result, uint32_t groups, bool bbox){
	PxRaycastHit hit;
	PxSceneQueryFilterData f;
	f.data.setToDefault();

	
	if(bbox){
		f.data.word3 = eIG_BBOX;
	}
	else{
		f.data.word0 = groups;
	}
	
	mScene->raycastSingle(PxVec3(origin.x, origin.y, origin.z),
		PxVec3(direction.x, direction.y, direction.z),
		PX_MAX_F32, PxSceneQueryFlag::eIMPACT|PxSceneQueryFlag::eDISTANCE, hit, f);

	if(hit.shape){
		PhysicsActor* actor = static_cast<PhysicsActor*>(hit.shape->getActor().userData);
		if(actor==NULL){
			return false;
		}

		result = RaycastHitEvent(actor, hit.distance, 0,
			glm::vec3(hit.impact.x, hit.impact.y, hit.impact.z));

		if(hit.shape->getGeometry().getType() == PxGeometryType::eHEIGHTFIELD){
			result.mTriangleIndex = hit.faceIndex;
		}

		return true;
	}
	else{
		return false;
	}
}

bool Physics::pick(math::Camera& camera, const math::Frustum& frustum, const glm::vec2& screenPos,
	const glm::vec2& screenSize, RaycastHitEvent& res, uint32_t groups, bool bbox){

	glm::mat4x4 modelView;
	camera.getMatrix(modelView);

	glm::vec3 point = math::unProject(screenPos.x, screenPos.y, screenSize.x,
		screenSize.y, modelView,
		frustum.getProjMatrix()
		);

	return pick(camera.getPosition(), glm::normalize(point-camera.getPosition()), res, groups, bbox);
}


Sp<PxGeometry> Physics::createGeometry(const PhysicsActor::Desc& desc){

	switch(desc.geometryType){
		case PhysicsActor::eBOX_GEOMETRY:
			return new PxBoxGeometry(
				desc.geometryDesc.boxGeometry.hx,
				desc.geometryDesc.boxGeometry.hy, 
				desc.geometryDesc.boxGeometry.hz);
			break;

		case PhysicsActor::eSPHERE_GEOMETRY:
			return new PxSphereGeometry(
				desc.geometryDesc.sphereGeometry.radius
				);
			break;

		case PhysicsActor::eMESH_GEOMETRY:
			return new PxTriangleMeshGeometry(
				cook( desc.geometryDesc.meshGeometry.model->getBatch() )
				);
			break;

		case PhysicsActor::eHEIGHTMAP_GEOMETRY:{
			 //Copy heightmap data
			const PhysicsActor::Desc::GeometryDesc::HeightfieldGeometry& hfDesc = desc.geometryDesc.heightfieldGeometry;

			uint32_t numRows = hfDesc.numRows;
			uint32_t numCols = hfDesc.numCols;

			Buffer<PxHeightFieldSample> samples(numRows*numCols);
			samples.clearMem();

			for(uint32_t row=0; row<numRows; row++){
				for(uint32_t col=0; col<numCols; col++){
					samples[row*numCols + col].height = (*hfDesc.heightmap)[row*numCols + col];
				}
			}

			// Create description
			PxHeightFieldDesc pxHfDesc;
			pxHfDesc.format = PxHeightFieldFormat::eS16_TM;
			pxHfDesc.nbColumns = numCols;
			pxHfDesc.nbRows = numRows;
			pxHfDesc.samples.data = samples.getData();
			pxHfDesc.samples.stride = sizeof(PxHeightFieldSample);

			WT_ASSERT(pxHfDesc.isValid(), "Invalid height field description");

			PxHeightField* heightfield = mSdk->createHeightField(pxHfDesc);

			return new PxHeightFieldGeometry(heightfield, PxMeshGeometryFlags(),
				hfDesc.heightScale, hfDesc.rowScale, hfDesc.colScale);
		};

		default:
			WT_THROW("Unsupported geometry type %d", desc.geometryType);
		}
}


PxController* Physics::createController(const PhysicsActor::Desc& desc){
	if(desc.controllerDesc.geometryType == PhysicsActor::eCAPSULE_CONTROLLER){
		// Capsule
		PxCapsuleControllerDesc pxDesc;

		pxConvert(desc.pose.getPosition(), pxDesc.position);

		pxDesc.radius = desc.controllerDesc.geometryDesc.capsuleController.radius;
		pxDesc.height = desc.controllerDesc.geometryDesc.capsuleController.height;
		pxDesc.material = mDefaultMaterial;

		WT_ASSERT( pxDesc.isValid(), "Invalid character controller desc");

		return mCtrlManager->createController(*mSdk, mScene, pxDesc);
	}
	else if(desc.controllerDesc.geometryType == PhysicsActor::eBOX_CONTROLLER){
		// Box
		PxBoxControllerDesc pxDesc;

		pxConvert(desc.pose.getPosition(), pxDesc.position);

		pxDesc.halfSideExtent = desc.controllerDesc.geometryDesc.boxController.hx;
		pxDesc.halfHeight = desc.controllerDesc.geometryDesc.boxController.hy;
		pxDesc.halfForwardExtent = desc.controllerDesc.geometryDesc.boxController.hz;

		pxDesc.material = mDefaultMaterial;

		WT_ASSERT( pxDesc.isValid(), "Invalid char controller desc");

		return mCtrlManager->createController(*mSdk, mScene, pxDesc);
	}
	else{
		WT_THROW("Invalid physics controller geometry type %d",
			desc.controllerDesc.geometryType
		);
	}
}

PhysicsActor* Physics::createActor(ASceneActor* sceneActor, PhysicsActor::Desc& desc){
	WT_ASSERT(desc.isValid(), "Invalid actor description");

	PxTransform pxTransform = PxTransform::createIdentity();
	pxConvert(desc.pose, pxTransform);

	PhysicsActor* createdActor = NULL;

	uint32_t actorId = generateActorId();

	PxFilterData filterData;
	filterData.setToDefault();

	filterData.word0 = desc.group;
	filterData.word1 = desc.collisionMask;

	if(desc.type == PhysicsActor::eREGION){
		Sp<PxGeometry> geometry = createGeometry(desc);

		PxRigidStatic* pxActor = mSdk->createRigidStatic(pxTransform);

		PxShape* shape = pxActor->createShape(*geometry, *mDefaultMaterial);

		filterData.setToDefault();
		filterData.word3 = eIG_REGION;

		shape->setQueryFilterData(filterData);
		shape->setSimulationFilterData(filterData);

		createdActor = new PhysicsActor(
			actorId, sceneActor ? sceneActor->getName() : "", desc.type, desc.controlMode, pxActor, NULL, sceneActor
			);

		pxActor->userData = static_cast<void*>(createdActor);

		mScene->addActor(*pxActor);
	}
	else if(desc.type == PhysicsActor::eSTATIC_ACTOR || desc.type == PhysicsActor::eACTOR_TYPE_BBOX){
		// static actor
		Sp<PxGeometry> geometry = createGeometry(desc);

		PxRigidStatic* pxActor = mSdk->createRigidStatic(pxTransform);

		PxShape* shape = pxActor->createShape(*geometry, *mDefaultMaterial);

		if(desc.type == PhysicsActor::eACTOR_TYPE_BBOX){
			filterData.word3 = eIG_BBOX;
		}
		else if(desc.geometryType == PhysicsActor::eHEIGHTMAP_GEOMETRY){
			// filter data used for heightmap only raycasts
			filterData.word3 = eIG_HEIGHTMAP;
		}

		
		shape->setQueryFilterData(filterData);
		shape->setSimulationFilterData(filterData);

		createdActor = new PhysicsActor(
			actorId, sceneActor ? sceneActor->getName() : "", desc.type, desc.controlMode, pxActor, NULL, sceneActor
			);

		pxActor->userData = static_cast<void*>(createdActor);

		mScene->addActor(*pxActor);

		if(sceneActor){
			if(desc.type == PhysicsActor::eACTOR_TYPE_BBOX){
				sceneActor->setBBox(createdActor);
			}
			else{
				sceneActor->setPhysicsActor(createdActor);
			}
		}
	}
	else if(desc.type == PhysicsActor::eDYNAMIC_ACTOR){
		// dynamic actor

		if(desc.controlMode == PhysicsActor::ePHYSICS_MODE){
			Sp<PxGeometry> geometry = createGeometry(desc);

			PxRigidDynamic* pxActor = mSdk->createRigidDynamic(pxTransform);

			// physics controlled
			if(desc.geometryType == PhysicsActor::eMESH_GEOMETRY){
				pxActor->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, false);		
			}

			PxShape* shape= pxActor->createShape(*geometry, *mDefaultMaterial);

			shape->setQueryFilterData(filterData);
			shape->setSimulationFilterData(filterData);

			createdActor = new PhysicsActor(
				actorId, sceneActor ? sceneActor->getName() : "", desc.type, desc.controlMode, pxActor, NULL, sceneActor
				);

			pxActor->userData = static_cast<void*>(createdActor);

			if(sceneActor){
				sceneActor->setPhysicsActor(createdActor);
			}

			mScene->addActor(*pxActor);
		}
		else if(desc.controlMode == PhysicsActor::eCONTROLLER_MODE){
			// controller bound
			PxController* controller = createController(desc);

			createdActor = new PhysicsActor(
				actorId, sceneActor ? sceneActor->getName() : "", desc.type, desc.controlMode, controller->getActor(), controller, sceneActor
				);

			PxShape* shape[1];
			
			controller->getActor()->getShapes((PxShape**)shape, 1);

			filterData.word0 = 0xAAAA;
			shape[0]->setQueryFilterData(filterData);
			shape[0]->setSimulationFilterData(filterData);

			controller->getActor()->userData =  static_cast<void*>(createdActor);

			if(sceneActor){
				sceneActor->setPhysicsActor(createdActor);
			}
		}
		else{
			WT_THROW("Invalid physics actor control mode %d", desc.controlMode
				);
		}
	}
	else{
		WT_THROW("Invalid physics actor type %d", desc.type
				);
	}

	if(createdActor){
		createdActor->setDesc(desc);

		mActors.insert(
			std::make_pair(actorId, createdActor)
		);
	}

	return createdActor;
}

void Physics::createBBox(ASceneActor* actor){
	PhysicsActor::Desc desc;

	desc.type = PhysicsActor::eACTOR_TYPE_BBOX;

	desc.geometryType = PhysicsActor::eBOX_GEOMETRY;

	desc.geometryDesc.boxGeometry.hx = 1.0f;
	desc.geometryDesc.boxGeometry.hy = 1.0f;
	desc.geometryDesc.boxGeometry.hz = 1.0f;

	desc.pose.setPosition( actor->getTransform().getPosition() );

	desc.collisionMask = 0;

	createActor(actor, desc);
}

LuaObject Physics::lua_getActorsInRegion(LuaObject luaPos, LuaObject luaRadius, LuaObject groupFilter){
	float radius;
	glm::vec3 pos;

	LuaObject res;
	LUA_NEW_TABLE(res);

	// TODO checks
	Lua::luaConv(luaPos, pos);
	Lua::luaConv(luaRadius, radius);

	Sp<Buffer<PhysicsActor*>> actors = getActorsInRegion(pos, radius, groupFilter.IsNil() ? 0x00 : groupFilter.ToInteger() );

	uint32_t actorIdx=0;
	for(uint32_t i=0; i<actors->getCapacity(); i++){
		ASceneActor* actor =  actors->operator[](i)->getSceneActor();
		if(actor){
			res.Set(++actorIdx, actor->getId());
		}
	}

	return res;
}

int32_t Physics::lua_createRegion(LuaObject luaPos, LuaObject luaRadius){
	glm::vec3 pos;
	float radius;

	if(!Lua::luaConv(luaPos, pos) || !Lua::luaConv(luaRadius, radius)){
		LOGE("Error creating region, invalid position or radius value");
		return -1;
	}

	return createRegion("", pos, radius);
}


void Physics::expose(LuaObject& meta){
	meta.RegisterObjectDirect("getActorsInRegion", (Physics*)0, &Physics::lua_getActorsInRegion);
	meta.RegisterObjectDirect("createRegion", (Physics*)0, &Physics::lua_createRegion);
}

}; // </wt>
