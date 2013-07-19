#ifndef WT_ASSIMPMODELLOADER_H
#define WT_ASSIMPMODELLOADER_H


#include "wt/stdafx.h"

#include "wt/Singleton.h"
#include "wt/Exception.h"
#include "wt/Buffer.h"
#include "wt/Common.h"
#include "wt/Model.h"
#include "wt/AResourceLoader.h"

namespace wt{

//#pragma comment(lib, "assimp.lib")

class AssimpModelLoader : public Singleton<AssimpModelLoader>{
public:

	typedef std::map<String, String> TextureMap;

	AssimpModelLoader();

	virtual ~AssimpModelLoader();

	void save(const String&, Model*);

	static bool nodeTreeHasMesh(aiNode* root);

	static aiNode* findSkeletonRoot(aiNode* node);

	static void buildSkeleton(SkeletonBone* bone, aiNode* aiBone, uint32_t& idx);

	static void buildSkeleton(Model& model, aiNode* aiRoot);

	void load(const String& filePath, Model& model, Animation* ani, TextureMap* texMap=NULL);

	void load(const String& filePath, Model* model);

private:
	static void convertAnimation(const aiAnimation* aAnim, Animation& anim);

	static void convertNodeAnim(const aiAnimation* aAnim, const aiNodeAnim* aNodeAnim, NodeAnimation& nodeAnim);

	static void aiConvert(const aiMatrix4x4& src, math::Transform& dst);

	static void aiConvert(const aiColor3D& src, Color& dst);

	static void aiConvert(const aiMaterial& src, Material& dst);

	static void aiConvert(const aiMatrix4x4& src, glm::mat4& dst);

}; // </AssimpModelLoader>

}; // </wt>

#undef TAG

#endif
