#ifndef WT_MODELLOADER_H
#define WT_MODELLOADER_H


#include "wt/stdafx.h"


#include "wt/ModelManager.h"

namespace wt{

class ModelLoader : public Singleton<ModelLoader>, public AResourceLoader<Model>{
private:
	static const char* TAG;
	static const char* FORMAT_ID;

	void writeNode(AIOStream* stream, SkeletonBone* node);

	void readNode(AIOStream* stream, Model* model, SkeletonBone* parent);

public:

	void load(AIOStream* stream, Model* model);

	void save(AIOStream* stream, Model* model);

	void create(Model* model);

	void postProcess(SkeletonBone* bone, const glm::mat4& transform);

	void postProcess(Model* model, const glm::mat4& transform);

}; // </ModelLoader>

}; // </wt>

#endif
