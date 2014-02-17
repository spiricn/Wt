#ifndef WT_SKYBOX_H
#define WT_SKYBOX_H


#include "wt/stdafx.h"

#include "wt/Utils.h"
#include "wt/GLBatch.h"
#include "wt/AResource.h"
#include "wt/AResourceManager.h"
#include "wt/Image.h"
#include "wt/ImageManager.h"
#include "wt/Singleton.h"
#include "wt/Transform.h"
#include "wt/AResourceLoader.h"

namespace wt
{

class SkyBoxLoader;

class SkyBox : public AResource<SkyBox>{
friend class SkyBoxLoader;
friend class SkyBoxManager;

public:
	enum Side{
		ePOS_X,
		eNEG_X,

		ePOS_Y,
		eNEG_Y,

		ePOS_Z,
		eNEG_Z
	};


protected:
private:
	typedef struct{
		GLfloat x,y,z;
	} Vertex;

	gl::Batch mBatch;
	GLuint mTexture;
	math::Transform mTransform;
	ImageLink mPosX, mNegX, mPosY, mNegY, mPosZ, mNegZ;

public:

	SkyBox(AResourceManager<SkyBox>* manager=NULL, ResourceHandle handle=0, const std::string& name="");

	~SkyBox();


	math::Transform& getTransform(){
		return mTransform;
	}

	void bind();

	const ImageLink& getSideImage(Side side) const{
		switch(side){
		case ePOS_X:
			return mPosX;
		case ePOS_Y:
			return mPosY;
		case ePOS_Z:
			return mPosZ;
		case eNEG_X:
			return mNegX;
		case eNEG_Y:
			return mNegY;
		case eNEG_Z:
			return mNegZ;
		default:
			WT_THROW("Invalid SkyBox side %d", side);
		}
	}

	void setImages(Image* posX, Image* negX,
		Image* posY, Image* negY, 
		Image* posZ, Image* negZ);

	void create();

	void serialize(lua::State* luaState, LuaPlus::LuaObject& dst);

	void deserialize(lua::State* luaState, const LuaPlus::LuaObject& src);

	gl::Batch& getBatch();
};




class SkyBoxManager : public AResourceManager<SkyBox>{
private:
	ImageManager* mImageManager;

public:
	SkyBoxManager(AResourceSystem* assets) : AResourceManager(assets){
	}
};

class SkyBoxLoader : public AResourceLoader<SkyBox>, public Singleton<SkyBoxLoader>{
public:
	void load(AIOStream* /*stream*/, SkyBox* /*dst*/){
	}

	void save(AIOStream* /*stream*/, SkyBox* /*src*/){
	}

	void create(SkyBox* sky){
		sky->create();
	}
};

typedef AResourceGroup<SkyBox> SkyBoxGroup;

}; // </wt>

#endif

