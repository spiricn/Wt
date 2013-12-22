#include "wt/stdafx.h"
#include "wt/PbSceneLoader.h"
#include "wt/Pb.h"
#include "wt/proto/Scene.pb.h"

#define TD_TRACE_TAG "PbSceneLoader"

namespace wt
{

template<class T>
class PbSerializable{
public:
	void serialize(T* dst);

	void deserialize(AResourceSystem* assets, const T& src, void* opaque);
}; // </PbSerializable>

PbSceneLoader::PbSceneLoader(AResourceSystem* assets, Scene* scene) : mScene(scene), mAssets(assets){
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

		if(actor.type() == pb::eTYPE_MODELLED){
			const pb::ModelledActor& pbModelledActor = actor.modelled_actor();

			ModelledActor* actor = mScene->createModelledActor(pbModelledActor.base().name());

			actor->deserialize(mAssets, pbModelledActor);
		}
		else if(actor.type() == pb::eTYPE_TERRAIN){
			const pb::Terrain& pbActor = actor.terrain();

			Terrain* actor = mScene->createTerrain(pbActor.base().name());

			actor->deserialize(mAssets, pbActor);
		}
		else{
			TRACEW("Not implemented");
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
		if(iter->second->getActorType() == ASceneActor::eTYPE_MODELLED){
			ModelledActor* actor = dynamic_cast<ModelledActor*>(iter->second);

			pb::SceneActor* pbActor = pbScene.mutable_actors()->Add();

			actor->serialize(pbActor->mutable_modelled_actor());

			pbActor->set_type(pb::eTYPE_MODELLED);
		}
		else if(iter->second->getActorType() == ASceneActor::eTYPE_TERRAIN){
			Terrain* actor = dynamic_cast<Terrain*>(iter->second);

			pb::SceneActor* pbActor = pbScene.mutable_actors()->Add();
			actor->serialize( pbActor->mutable_terrain() );

			pbActor->set_type(pb::eTYPE_TERRAIN);
		}
		else{
			TRACEW("Not implemented");
		}
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

} // </wt>

