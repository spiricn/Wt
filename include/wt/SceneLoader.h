#pragma once
#ifndef WT_SCENELOADER_H
#define WT_SCENELOADER_H

#include "wt/AResourceSystem.h"
#include "wt/Scene.h"

namespace wt{

class SceneLoader{
public:
	SceneLoader(Scene* scene, AResourceSystem* assets);

	void load(AIOStream& stream);

	void save(AIOStream& stream);

	void load(const String& uri);

	void save(const String& uri);

private:
	Scene* mScene;
	AResourceSystem* mAssets;

}; // </SceneLoader>

}; // </wt>

#endif // </WT_SCENELOADER_H>