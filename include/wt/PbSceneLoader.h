#ifndef WT_PBSCENELOADER_H
#define WT_PBSCENELOADER_H

#include "wt/ISceneLoader.h"

namespace wt
{

class AResourceSystem;
class Scene;
class PBActorFactory;

class PbSceneLoader{
public:
	PbSceneLoader(AResourceSystem* assets, Scene* scene);

	~PbSceneLoader();

	void load(AIOStream& stream);

	void save(AIOStream& stream);

	void load(const String& uri);

	void save(const String& uri);

private:
	Scene* mScene;
	AResourceSystem* mAssets;
	PBActorFactory* mFactory;


}; // </PbSceneLoader>

} // </wt>

#endif // </WT_PBSCENELOADER_H>
