#include "wt/stdafx.h"
#include "wt/ModelSkin.h"

#define TD_TRACE_TAG "ModelSkin"

namespace wt
{

ModelSkin::ModelSkin(Model* model, const String& name) : mModel(model), mName(name){
}

ModelSkin::~ModelSkin(){
	for(MeshList::iterator i=mMeshes.begin(); i!=mMeshes.end(); i++){
		delete *i;
	}

	mMeshes.clear();
}

ModelSkin::Mesh* ModelSkin::findMeshByName(const String& name){
	for(MeshList::iterator i=mMeshes.begin(); i!=mMeshes.end(); i++){
		if(name.compare((*i)->geometry->getName())==0){
			return *i;
		}
	}

	return NULL;
}

const String& ModelSkin::getName() const{
	return mName;
}

Model* ModelSkin::getModel(){
	return mModel;
}

ModelSkin::MeshList::iterator ModelSkin::getMeshListBeg(){
	return mMeshes.begin();
}

ModelSkin::MeshList::iterator ModelSkin::getMeshListEnd(){
	return mMeshes.end();
}

ModelSkin::Mesh* ModelSkin::findMeshByGeometry(Geometry* geometry){
	for(MeshList::iterator i=mMeshes.begin(); i!=mMeshes.end(); i++){
		if((*i)->geometry == geometry){
			return *i;
		}
	}

	return NULL;
}


void ModelSkin::deleteMesh(Mesh* mesh){
	MeshList::iterator iter = std::find(mMeshes.begin(), mMeshes.end(), mesh);
	WT_ASSERT(iter != mMeshes.end(), "Invalid model mesh");

	mMeshes.erase(iter);

	delete mesh;
}

ModelSkin::Mesh* ModelSkin::createMesh(Geometry* geometry){
	Mesh* mesh = new Mesh(geometry);
	mMeshes.push_back(mesh);

	return mesh;
}

} // </wt>

