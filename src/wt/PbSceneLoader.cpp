#include "wt/stdafx.h"
#include "wt/PbSceneLoader.h"
#include "wt/Pb.h"
#include "wt/proto/Scene.pb.h"

#define TD_TRACE_TAG "PbSceneLoader"

namespace wt
{

#if 0

class IPBActorFactory{
public:
	virtual ASceneActor::ActorType getActorType() const = 0;

	virtual ASceneActor* load(const pb::SceneActor& src) = 0;

	virtual void save(const ASceneActor& src, pb::SceneActor* dst) = 0;

	IPBActorFactory(Scene* scene, AResourceSystem* resourceSystem) : mScene(scene), mResourceSystem(resourceSystem){
	}

	virtual ~IPBActorFactory(){
	}

	Scene* getScene() const{
		return mScene;
	}

	AResourceSystem* getAssets() const{
		return mResourceSystem;
	}

private:
	Scene* mScene;
	AResourceSystem* mResourceSystem;
};

class ModelledActorFactory : public IPBActorFactory{
public:
	ASceneActor::ActorType getActorType() const{
		return ASceneActor::eTYPE_MODELLED;
	}

	ASceneActor* load(const pb::SceneActor& aSrc){
		WT_ASSERT(aSrc.has_modelled_actor(), "Invalid modelled actor description");

		// Get the description
		const pb::ModelledActor& src = aSrc.modelled_actor();

		// Create the actor
		ModelledActor* actor = getScene()->createModelledActor(src.base().name());

		// Assemble it
		ModelledActor::DeserializationData data;
		actor->deserialize(getAssets(), src, &data);

		if(data.phyiscs){
			// TODO
			TRACEW("NOT IMPLEMENTED");
		}

		return actor;
	}

	void save(const ASceneActor& aActor, pb::SceneActor* pbActor){
		const ModelledActor& actor = dynamic_cast<const ModelledActor&>(aActor);

		actor.serialize(pbActor->mutable_modelled_actor());
	}

	ModelledActorFactory(Scene* scene, AResourceSystem* resourceSystem) : IPBActorFactory(scene, resourceSystem){
	}
};


 
class TerrainFactory : public IPBActorFactory{
public:
	ASceneActor::ActorType getActorType() const{
		return ASceneActor::eTYPE_TERRAIN;
	}

	ASceneActor* load(const pb::SceneActor& aSrc){
		WT_ASSERT(aSrc.has_terrain(), "Invalid modelled actor description");

		const pb::Terrain& pbActor = aSrc.terrain();

		// Create the scene actor
		Terrain* actor = getScene()->createTerrain(pbActor.base().name());

		actor->deserialize(getAssets(), pbActor);

		// Create the physics actor
		PhysicsActor::Desc desc;
		actor->getPhysicsDesc(desc);

		getScene()->getPhysics()->createActor(actor, desc);

		return actor;
	}

	void save(const ASceneActor& aActor, pb::SceneActor* pbActor){
		const Terrain& actor = dynamic_cast<const Terrain&>(aActor);

		actor.serialize( pbActor->mutable_terrain() );
	}

	TerrainFactory(Scene* scene, AResourceSystem* resourceSystem) : IPBActorFactory(scene, resourceSystem){
	}
};


class ParticleFactory : public IPBActorFactory{
public:
	ASceneActor::ActorType getActorType() const{
		return ASceneActor::eTYPE_PARTICLE_EFFECT;
	}

	ASceneActor* load(const pb::SceneActor& aSrc){
		WT_ASSERT(aSrc.has_particle_effect(), "Invalid actor description");

		const pb::ParticleEffect& pbActor = aSrc.particle_effect();

		// Create the scene actor
		ParticleEffect* actor = getScene()->createParticleEffect(pbActor.base().name());

		// TODO
		//actor->deserialize(getAssets(), pbActor);

		return actor;
	}

	void save(const ASceneActor& aActor, pb::SceneActor* pbActor){
		const ParticleEffect& actor = dynamic_cast<const ParticleEffect&>(aActor);

		// TODO
		//actor.serialize( pbActor->mutable_particle_effect() );
	}

	ParticleFactory(Scene* scene, AResourceSystem* resourceSystem) : IPBActorFactory(scene, resourceSystem){
	}
};

template<class ActorType, class DescType>
class GenericActorFactory{
public:
	GenericActorFactory(Scene* scene, AResourceSystem* resourceSystem, ASceneActor::ActorType type) : mScene(scene), mAssets(resourceSystem), mActorType(type){
	}

	ActorType* load(const pb::SceneActor& src){
		// Create the scene actor
		ActorType* actor = getScene()->createActor(mActorType, pbActor.base().name());

		// src
		actor->deserialize(getAssets(), src);

		// TODO physics

		return actor;
	}

	void save(const ASceneActor& actor, pb::SceneActor* pbActor){
		actor->serialize(pbActor);
	}

	ASceneActor::ActorType getActorType() const{
		return mActorType;
	}

private:
	ASceneActor::ActorType mActorType;
	Scene* mScene;
	AResourceSystem* mAssets;
};

class PBActorFactory{
public:
	PBActorFactory(AResourceSystem* assets, Scene* scene){
		registerFactory( new ModelledActorFactory(scene, assets) );
		registerFactory( new TerrainFactory(scene, assets) );
		registerFactory( new ParticleFactory(scene, assets) );
	}

	
	////ActorType* load

	~PBActorFactory(){
		for(FactoryMap::iterator iter=mFactories.begin(); iter!=mFactories.end(); iter++){
			delete iter->second;
		}
	}

	ASceneActor* load(const pb::SceneActor& src){
		IPBActorFactory* factory = getFactory( pb::conv(src.type()) );

		if(factory == NULL){
			TRACEE("No factory found for actor type %d", src.type());
			return NULL;
		}

		return factory->load(src);
	}

	void save(const ASceneActor& aActor, pb::SceneActor* pbActor){
		IPBActorFactory* factory = getFactory( aActor.getActorType() );

		WT_ASSERT(factory != NULL, "No factory found for actor type %d", aActor.getActorType());

		factory->save(aActor, pbActor);
	}


private:
	void registerFactory(IPBActorFactory* factory){
		WT_ASSERT(getFactory(factory->getActorType()) == NULL, "Factory already registered for actor type %d", factory->getActorType());

		mFactories.insert(std::make_pair(factory->getActorType(), factory));
	}

	IPBActorFactory* getFactory(ASceneActor::ActorType type){
		FactoryMap::iterator iter = mFactories.find(type);
		return iter == mFactories.end() ? NULL : iter->second;
	}

	typedef std::map<ASceneActor::ActorType, IPBActorFactory*> FactoryMap;

	FactoryMap mFactories;
};

PbSceneLoader::PbSceneLoader(AResourceSystem* assets, Scene* scene) : mScene(scene), mAssets(assets){
	mFactory = new PBActorFactory(assets, scene);
}


PbSceneLoader::~PbSceneLoader(){
}

void PbSceneLoader::load(AIOStream& stream){
	// Read entire stream into a buffer
	uint8_t* bfr = new uint8_t[stream.getSize()];
	stream.read(bfr, stream.getSize());

	pb::Scene pbScene;
	pbScene.ParseFromArray(bfr, stream.getSize());


	// Actors
	for(int i=0; i<pbScene.actors().size(); i++){
		const pb::SceneActor& actor = pbScene.actors().Get(i);

		try{
			mFactory->load(actor);
		}catch(...){
			TRACEE("Error loading actor");
		}
	}

	// TODO Lights

	// Skybox
	if(pbScene.has_skybox()){
		mScene->setSkyBox( mAssets->getSkyBoxManager()->getFromPath(pbScene.skybox()) );
	}
	
	// TODO Godray

	// TODO Fog

	
	// Camera
	{
		glm::vec3 translation;
		glm::quat rotation;
		pb::conv(pbScene.camera().rotation(), &rotation);
		pb::conv(pbScene.camera().position(), &translation);

		mScene->getCamera().setRotation(rotation);
		mScene->getCamera().setTranslation(translation);
	}
}

void PbSceneLoader::save(AIOStream& stream){
	pb::Scene pbScene;

	// Actors
	for(Scene::ActorMap::iterator iter=mScene->getActorMap().begin(); iter!=mScene->getActorMap().end(); iter++){
		pb::SceneActor* actor = pbScene.mutable_actors()->Add();

		mFactory->save(*iter->second, actor);

		actor->set_type(pb::conv(iter->second->getActorType()));
	}

	// Skybox
	if(mScene->getSkyBox()){
		*pbScene.mutable_skybox() = mScene->getSkyBox()->getPath();
	}

	// TODO Lights

	// TODO Godray

	// TODO Fog

	// Camera
	{
		glm::vec3 translation;
		glm::quat rotation;
		mScene->getCamera().getRotation(rotation);
		mScene->getCamera().getTranslation(translation);
		pb::conv(translation, pbScene.mutable_camera()->mutable_position());
		pb::conv(rotation, pbScene.mutable_camera()->mutable_rotation());
	}
	

	// Serialize
	uint8_t* bfr = new uint8_t[ pbScene.ByteSize() ];
	pbScene.SerializeToArray(bfr, pbScene.ByteSize());

	stream.write(bfr, pbScene.ByteSize());
}

void PbSceneLoader::load(const String& uri){
	load( *mAssets->getFileSystem()->open(uri, AIOStream::eMODE_READ).get() );
}

void PbSceneLoader::save(const String& uri){
	save( *mAssets->getFileSystem()->open(uri, AIOStream::eMODE_WRITE).get() );
}

#endif

} // </wt>

