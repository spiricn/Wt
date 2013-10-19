#include "stdafx.h"

#include "wte/ProjectScene.h"

#define TD_TRACE_TAG "ProjectScene"

ProjectScene::ProjectScene(Project* project, Id id) : mId(id), mPath(""), mProject(project){
}

Project* ProjectScene::getProject() const{
	return mProject;
}

ProjectScene::Id ProjectScene::getId() const{
	return mId;
}

void ProjectScene::setPath(const QString& path){
	mPath = path;
}

const QString& ProjectScene::getPath() const{
	return mPath;
}

void ProjectScene::serialize(wt::lua::State* luaState, LuaPlus::LuaObject& dst) const{
	dst.Set("id", mId);
	dst.Set("path", mPath.toStdString().c_str());
}

void ProjectScene::deserialize(wt::lua::State* luaState, const LuaPlus::LuaObject& src){
	mPath = src.Get("path").ToString();
	mId = src.Get("id").ToInteger();
}