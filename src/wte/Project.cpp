#include "stdafx.h"

#include "wte/Project.h"

#include <wt/lua/State.h>

#define TD_TRACE_TAG "Project"

Project::Project(){
}

void Project::removeScene(ProjectScene::Id sceneId){
}

void Project::save(const QString& path){
	wt::lua::State state(false);
	wt::lua::ScriptPtr script =  state.createScript(path.toStdString().c_str());

	LuaPlus::LuaObject scenesTable;
	state.assignTable(scenesTable);

	int32_t idx=0;
	for(SceneSet::iterator iter=mSceneSet.begin(); iter!=mSceneSet.end(); iter++){
		LuaPlus::LuaObject sceneTable;
		state.assignTable(sceneTable);

		(*iter)->serialize(&state, sceneTable);

		scenesTable.Set(++idx, sceneTable);
	}

	state.getGlobals().Set("scenes", scenesTable);
	state.getGlobals().Set("assets_root", mAssetsRoot.toStdString().c_str());
}

void Project::load(const QString& path){
	mSceneSet.clear();

	for(SceneMap::iterator iter=mScenes.begin(); iter!=mScenes.end(); iter++){
		delete iter->second;
	}

	mScenes.clear();

	wt::lua::State state(false);
	wt::lua::ScriptPtr script =  state.createScript(path.toStdString().c_str());

	for(LuaPlus::LuaTableIterator iter( script->getState().Get("scenes") ); iter; iter.Next()){
		ProjectScene* scene = new ProjectScene(this);

		scene->deserialize(&state, iter.GetValue());

		mSceneSet.insert(scene);
		mScenes.insert(std::make_pair(scene->getId(), scene));
	}

	mAssetsRoot = script->getState().Get("assets_root").ToString();
}

const QString& Project::getAssetsRoot() const{
	return mAssetsRoot;
}

const Project::SceneSet& Project::getSceneSet() const{
	return mSceneSet;
}
