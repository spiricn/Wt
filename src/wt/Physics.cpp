#include "wt/stdafx.h"

#include "wt/Physics.h"
#include "wt/camera.h"

#define TD_TRACE_TAG "Physics"

namespace wt
{

enum InternalGroups{
	eIG_HEIGHTMAP = (1 << 0),
	eIG_BBOX = (1 << 1)
}; // </EngineGroups>

// TODO move this to Physics class
static bool handleCollision(PxFilterData filterData0, PxFilterData filterData1);

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
    mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocator, *this);
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
    mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams(PxTolerancesScale()));
		
    if(mCooking == NULL){
        WT_THROW("Error creating cooking interface");
    }
        
    // Scene
    PxSceneDesc sceneDesc(mSdk->getTolerancesScale());

	// Filter shader
    mDefaultFilterShader = filterShader;
    sceneDesc.filterShader = mDefaultFilterShader;
	
    sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
    sceneDesc.cpuDispatcher = mCpuDispatcher;
    sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVETRANSFORMS | PxSceneFlag::eENABLE_KINEMATIC_PAIRS | PxSceneFlag::eENABLE_KINEMATIC_STATIC_PAIRS ;
	sceneDesc.simulationEventCallback = this;

    if(!sceneDesc.isValid()){
        WT_THROW("Invalid Px scene description");
    }		

    mScene = mSdk->createScene(sceneDesc);
    if(mScene == NULL){
        WT_THROW("Error creating PxScene");
    }


    // Character controler
    mCtrlManager = PxCreateControllerManager(*mScene);
    if(mCtrlManager == NULL){
        WT_THROW("Error creating PxCharacterController");
    }

    // material
    mDefaultMaterial = mSdk->createMaterial(0.5, 0.5, 0.5);

    // ground plane
    PxTransform pose = PxTransform(PxVec3(0.0f, -200.0f, 0.0f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f))
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

	// Let triggers through
	if(PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1)){
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}

	if(handleCollision(filterData0, filterData1)){
		// Collision occured
		pairFlags |= PxPairFlag::eCONTACT_DEFAULT /* allow collision, no need for callbacks yet*/;
	}
	
	return PxFilterFlag::eDEFAULT;
}


uint32_t Physics::createRegion(const String& name, const glm::vec3& position, float radius){
	PhysicsActor::Desc desc;

	desc.type = PhysicsActor::eTYPE_REGION;
	desc.geometryType = PhysicsActor::eGEOMETRY_SPHERE;
	desc.geometryDesc.sphereGeometry.radius = radius;
	desc.pose = glm::translate(position);

	PhysicsActor* actor = createActor(NULL, desc);

	mRegions.insert(actor);

	return actor->getId();
}

void Physics::deleteRegion(uint32_t region){
	PhysicsActor* actor = getActorById(region);

	WT_ASSERT(actor != NULL && actor->getType() == PhysicsActor::eTYPE_REGION, "Invalid region id %u", region);

	removeActor(actor);

	mRegions.erase(actor);
}

bool Physics::connectToVisualDebugger(const String& addr, int32_t port, int32_t timeout){
	#ifdef PX_SUPPORT_VISUAL_DEBUGGER
	// Physx Visual Debugger
	if(mSdk->getPvdConnectionManager() == NULL){
		WT_THROW("Unable to get PVD connection manager");
	}
	else{
		LOGV("Connecting to PVD %s : %d ...", addr.c_str(), port);


		physx::PxVisualDebuggerConnection* theConnection = PxVisualDebuggerExt::createConnection(mSdk->getPvdConnectionManager(),
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
    const PxActiveTransform* activeTransforms = mScene->getActiveTransforms(numActiveTrans);

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

			// Since we're the controller of the object we need to const cast this in order to change it
			if(actor->getSceneActorCtrl()){
				// Translation
				(const_cast<ASceneActor*>(actor->getSceneActor()))->physicsControl(glm::vec3(pos.x, pos.y, pos.z));
			}
		}
		else if(actor->getSceneActor()){
			// Since we're the controller of the object we need to const cast this in order to change it
			glm::vec3 pos;
			glm::quat rot;

			pxConvert(activeTransforms[i].actor2World.p, pos);
			pxConvert(activeTransforms[i].actor2World.q, rot);

			(const_cast<ASceneActor*>(actor->getSceneActor()))->physicsControl(pos);

			(const_cast<ASceneActor*>(actor->getSceneActor()))->physicsControl(rot);

		}
		else{
			LOGW("No scene actor associated with physx actor!");
		}
    }
}

void Physics::setTimeStep(float step){
	mTimeAccumulator.setStep(step);
}


Sp<Buffer<PhysicsActor*>> Physics::getActorsInRegion(const glm::vec3& pos, float radius, uint32_t groups){
#if 0
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
#else
	WT_THROW("NOT IMPLEMENTED");
#endif
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
		return hit.position.y;
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
#if 0
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
#else
	WT_THROW("Not implemented");
#endif
}

PxTriangleMesh* Physics::cook(gl::Batch& src){
	WT_ASSERT(
		src.isGenerated(), "Can not cook ungenerated batch");
	PxBoundedData vertices;

	vertices.data = src.getVertexBuffer().map(gl::Buffer::eREAD_ONLY);
    vertices.count = src.getNumVertices();
    vertices.stride = src.getVertexSize();

	PxBoundedData indices;

	indices.data = src.getIndexBuffer().map(gl::Buffer::eREAD_ONLY);
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


bool Physics::pick(const glm::vec3& origin, const glm::vec3& direction, RaycastHitEvent& result, uint32_t groups, PickFlag flags){
	PxRaycastHit hit;

	PxSceneQueryFilterData f;
	f.data.setToDefault();

	if(flags & ePICK_BOUNDING_BOXES){
		f.data.word3 |= eIG_BBOX;
	}

	if(flags & ePICK_TERRAIN){
		f.data.word3 |= eIG_HEIGHTMAP;
	}

	if(flags & ePICK_ACTORS){
		f.data.word0 = groups;
	}
	
	mScene->raycastSingle(PxVec3(origin.x, origin.y, origin.z),
		PxVec3(direction.x, direction.y, direction.z),
		PX_MAX_F32, PxSceneQueryFlag::eIMPACT|PxSceneQueryFlag::eDISTANCE, hit, f);

	if(hit.shape){
		PhysicsActor* actor = static_cast<PhysicsActor*>(hit.shape->getActor()->userData);
		if(actor==NULL){
			return false;
		}

		result = RaycastHitEvent(actor, hit.distance, 0,
			glm::vec3(hit.position.x, hit.position.y, hit.position.z));

		if(hit.shape->getGeometry().getType() == PxGeometryType::eHEIGHTFIELD){
			result.mTriangleIndex = hit.faceIndex;
		}

		return true;
	}
	else{
		return false;
	}
}

bool Physics::pick(math::Camera& camera, const glm::vec2& screenPos,
	const glm::vec2& screenSize, RaycastHitEvent& res, uint32_t groups, PickFlag flags){

	glm::mat4x4 modelView;
	camera.getCameraMatrix(modelView);

	glm::vec3 point = math::unProject(screenPos.x, screenPos.y, screenSize.x,
		screenSize.y, modelView,
		camera.getProjectionMatrix()
		);

	glm::vec3 eyePos;
	camera.getTranslation(eyePos);

	return pick(eyePos, glm::normalize(point-eyePos), res, groups, flags);
}


Sp<PxGeometry> Physics::createGeometry(const PhysicsActor::Desc& desc){

	if(desc.geometryType == PhysicsActor::eGEOMETRY_BOX){
		PxBoxGeometry* res = new PxBoxGeometry(
			desc.geometryDesc.boxGeometry.hx,
			desc.geometryDesc.boxGeometry.hy, 
			desc.geometryDesc.boxGeometry.hz);

		WT_ASSERT(res->isValid(), "Invalid box geometry provided");

		return res;
	}
	else if(desc.geometryType == PhysicsActor::eGEOMETRY_SPHERE){
		PxSphereGeometry* res = new PxSphereGeometry(
			desc.geometryDesc.sphereGeometry.radius
		);

		WT_ASSERT(res->isValid(), "Invalid sphere geometry provided");

		return res;
	}
	else if(desc.geometryType == PhysicsActor::eGEOMETRY_MESH){
		PxTriangleMeshGeometry* res = new PxTriangleMeshGeometry(
				cook( desc.geometryDesc.meshGeometry.model->getBatch() )
		);

		WT_ASSERT(res->isValid(), "Invalid mesh geometry provided");

		return res;
	}
	else if(desc.geometryType == PhysicsActor::eGEOMETRY_PLANE){
		return new PxPlaneGeometry;
	}
	else if(desc.geometryType == PhysicsActor::eGEOMETRY_HEIGHTMAP){
		//Copy heightmap data
		const PhysicsActor::Desc::GeometryDesc::HeightfieldGeometry& hfDesc = desc.geometryDesc.heightfieldGeometry;

		uint32_t numRows = hfDesc.numRows;
		uint32_t numCols = hfDesc.numCols;

		Buffer<PxHeightFieldSample> samples(numRows*numCols);
		samples.clearMem();

		for(uint32_t row=0; row<numRows; row++){
			for(uint32_t col=0; col<numCols; col++){
				samples[row*numCols + col].height = hfDesc.heightmap->getSamples()[row*numCols + col];
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

		PxHeightFieldGeometry* res = new PxHeightFieldGeometry(heightfield, PxMeshGeometryFlags(),
			hfDesc.heightScale, hfDesc.rowScale, hfDesc.colScale);

		WT_ASSERT(res->isValid(), "Invalid heightfield provided geometry");

		return res;
	}
	else{
		WT_THROW("Unsupported geometry type %d", desc.geometryType);
	}
}


PxController* Physics::createController(const PhysicsActor::Desc& desc){
	glm::vec3 pos;
	math::extractTranslation(desc.pose, pos);

	if(desc.controllerDesc.geometryType == PhysicsActor::eCTRL_GEOMETRY_CAPSULE){
		// Capsule
		PxCapsuleControllerDesc pxDesc;

		pxConvert(pos, pxDesc.position);

		PxControllerFilterCallback;

		pxDesc.radius = desc.controllerDesc.geometryDesc.capsuleController.radius;
		pxDesc.height = desc.controllerDesc.geometryDesc.capsuleController.height;
		pxDesc.material = mDefaultMaterial;

		WT_ASSERT( pxDesc.isValid(), "Invalid character controller desc");

		return mCtrlManager->createController(pxDesc);
	}
	else if(desc.controllerDesc.geometryType == PhysicsActor::eCTRL_GEOMETRY_BOX){
		// Box
		PxBoxControllerDesc pxDesc;

		pxConvert(pos, pxDesc.position);

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

	if(desc.type == PhysicsActor::eTYPE_REGION){
		Sp<PxGeometry> geometry = createGeometry(desc);

		PxRigidStatic* pxActor = mSdk->createRigidStatic(pxTransform);

		PxShape* shape = pxActor->createShape(*geometry, *mDefaultMaterial);

		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

		shape->setQueryFilterData(filterData);
		shape->setSimulationFilterData(filterData);

		createdActor = new PhysicsActor(this,
			actorId, sceneActor ? sceneActor->getName() : "", desc.type, desc.controlMode, pxActor, NULL, sceneActor
			);

		pxActor->userData = static_cast<void*>(createdActor);
		
		if(sceneActor){
			sceneActor->setPhysicsActor(createdActor);
		}

		mScene->addActor(*pxActor);
	}
	else if(desc.type == PhysicsActor::eTYPE_STATIC || desc.type == PhysicsActor::eTYPE_BOUNDING_BOX){
		// static actor
		Sp<PxGeometry> geometry = createGeometry(desc);

		PxRigidStatic* pxActor = mSdk->createRigidStatic(pxTransform);

		PxShape* shape = pxActor->createShape(*geometry, *mDefaultMaterial);

		if(desc.type == PhysicsActor::eTYPE_BOUNDING_BOX){
			filterData.word3 = eIG_BBOX;
			shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		}
		else if(desc.geometryType == PhysicsActor::eGEOMETRY_HEIGHTMAP){
			// filter data used for heightmap only raycasts
			filterData.word3 = eIG_HEIGHTMAP;
		}

		
		shape->setQueryFilterData(filterData);
		shape->setSimulationFilterData(filterData);

		createdActor = new PhysicsActor(this,
			actorId, sceneActor ? sceneActor->getName() : "", desc.type, desc.controlMode, pxActor, NULL, sceneActor
			);

		pxActor->userData = static_cast<void*>(createdActor);

		mScene->addActor(*pxActor);

		if(sceneActor){
			if(desc.type == PhysicsActor::eTYPE_BOUNDING_BOX){
				sceneActor->setBBox(createdActor);
			}
			else{
				sceneActor->setPhysicsActor(createdActor);
			}
		}
	}
	else if(desc.type == PhysicsActor::eTYPE_DYNAMIC){
		// dynamic actor

		if(desc.controlMode == PhysicsActor::eCTRL_MODE_PHYSICS){
			Sp<PxGeometry> geometry = createGeometry(desc);

			PxRigidDynamic* pxActor = mSdk->createRigidDynamic(pxTransform);

			// physics controlled
			if(desc.geometryType == PhysicsActor::eGEOMETRY_MESH){
				pxActor->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, false);		
			}

			PxShape* shape= pxActor->createShape(*geometry, *mDefaultMaterial);

			shape->setQueryFilterData(filterData);
			shape->setSimulationFilterData(filterData);

			createdActor = new PhysicsActor(this,
				actorId, sceneActor ? sceneActor->getName() : "", desc.type, desc.controlMode, pxActor, NULL, sceneActor
				);

			pxActor->userData = static_cast<void*>(createdActor);

			if(sceneActor){
				sceneActor->setPhysicsActor(createdActor);
			}

			mScene->addActor(*pxActor);
		}
		else if(desc.controlMode == PhysicsActor::eCTRL_MODE_CONTROLLER){
			// controller bound
			PxController* controller = createController(desc);

			createdActor = new PhysicsActor(this,
				actorId, sceneActor ? sceneActor->getName() : "", desc.type, desc.controlMode, controller->getActor(), controller, sceneActor
				);

			PxShape* shape[1];
			
			controller->getActor()->getShapes((PxShape**)shape, 1);

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

		if(sceneActor && desc.sceneActorCtrl){
			// Initial physics control
			glm::vec3 pos;
			glm::quat rot;

			PxTransform pose = static_cast<PxRigidActor*>(createdActor->getPxActor())->getGlobalPose();
			pxConvert(pose.p, pos);
			pxConvert(pose.q, rot);

			sceneActor->physicsControl(pos);

			// Only control rotation for non-controlled actors
			if(!createdActor->isControlled()){
				sceneActor->physicsControl(rot);
			}
		}
	}

	return createdActor;
}

void Physics::createBBox(ASceneActor* actor){
	PhysicsActor::Desc desc;

	desc.type = PhysicsActor::eTYPE_BOUNDING_BOX;

	desc.geometryType = PhysicsActor::eGEOMETRY_BOX;

	desc.geometryDesc.boxGeometry.hx = 1.0f;
	desc.geometryDesc.boxGeometry.hy = 1.0f;
	desc.geometryDesc.boxGeometry.hz = 1.0f;

	glm::vec3 pos;
	actor->getTransformable()->getTranslation(pos);

	desc.pose = glm::translate(pos);

	desc.collisionMask = 0;
	desc.group = 0;

	// Bounding box actor does NOT control a scene actor (other way around)
	desc.sceneActorCtrl = false;

	PhysicsActor* pxActor = createActor(actor, desc);
}

Physics::PickFlag operator|(Physics::PickFlag a, Physics::PickFlag b){
	return static_cast<Physics::PickFlag>( static_cast<int>(a) | static_cast<int>(b) );
}

PxQueryHitType::Enum Physics::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags){

	
	const PxFilterData filterData1 = shape->getSimulationFilterData();

	
	if(handleCollision(filterData, filterData1)){
		return PxQueryHitType::eTOUCH;
	}
	else{
		return PxQueryHitType::eNONE;
	}
}

PxQueryHitType::Enum Physics::postFilter(const PxFilterData& filterData, const PxQueryHit& hit){
	return PxQueryHitType::eTOUCH;
}

void Physics::reportError(PxErrorCode::Enum code, const char* message, const char* file, int line){
	if( code != 128 /* static actor moved (safe to ignore)*/){
		TRACEE("Error (code %d): \"%s\" at %s:%d", code, message, file, line);
	}
}

void Physics::onContact(const PxContactPairHeader&, const PxContactPair*, PxU32){
}

void Physics::onTrigger(PxTriggerPair* pairs, PxU32 count){
	for(int i=0; i<count; i++){
		const PxTriggerPair& pair = pairs[i];

		if(pair.status & PxPairFlag::eNOTIFY_TOUCH_FOUND || pair.status & PxPairFlag::eNOTIFY_TOUCH_LOST){
			PhysicsActor* actor1 = static_cast<PhysicsActor*>( pair.triggerActor->userData );

			PhysicsActor* actor2 = static_cast<PhysicsActor*>( pair.otherActor->userData );

			PhysicsActor* regionActor = actor1->getType() == PhysicsActor::eTYPE_REGION ? actor1 : actor2;

			PhysicsActor* collidingActor = actor1->getType() == PhysicsActor::eTYPE_REGION ? actor2 : actor1;

			// Generate trigger event
			RegionEvent* evt = new RegionEvent(
				regionActor,
				collidingActor,
				pair.status & PxPairFlag::eNOTIFY_TOUCH_FOUND ? RegionEvent::eACTOR_ENTERED_REGION : RegionEvent::eACTOR_LEFT_REGION
			);

			mEventManager->queueEvent(evt);
		}
	}
}

void Physics::onConstraintBreak(PxConstraintInfo*, PxU32){
}

void Physics::onWake(PxActor** , PxU32 ){
}

void Physics::onSleep(PxActor** , PxU32 ){
}

const Physics::ActorSet& Physics::getRegionSet() const{
	return mRegions;
}

bool handleCollision(PxFilterData filterData0, PxFilterData filterData1){
	return (filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1);
}

const EvtType RaycastHitEvent::TYPE = "RaycastHit";

const EvtType RegionEvent::TYPE = "RegionEvent";

}; // </wt>
