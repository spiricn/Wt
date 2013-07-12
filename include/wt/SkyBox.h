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

namespace wt{

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

	void setImageManager(ImageManager* imageManager){
		mImageManager = imageManager;
	}

private:
	ImageManager* mImageManager;
	typedef struct{
		GLfloat x,y,z;
	} Vertex;

	Gl::Batch mBatch;
	GLuint mTexture;
	math::Transform mTransform;
	ImageLink mPosX, mNegX, mPosY, mNegY, mPosZ, mNegZ;

public:

	SkyBox(ResourceHandle handle=0, const std::string& name="");

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
			WT_EXCEPT("SkyBox", "Invalid SkyBox side %d", side);
		}
	}

	void setImages(Image* posX, Image* negX,
		Image* posY, Image* negY, 
		Image* posZ, Image* negZ);

	void create();

	void serialize(LuaPlus::LuaObject& dst);

	void deserialize(const LuaPlus::LuaObject& src);

	Gl::Batch& getBatch();
};




class SkyBoxManager : public AResourceManager<SkyBox>{
private:
	ImageManager* mImageManager;

public:
	SkyBoxManager(ImageManager* imageManager) : mImageManager(imageManager){
	}

	SkyBox* create(const String& name){
		SkyBox* res = AResourceManager<SkyBox>::create(name);

		res->setImageManager(mImageManager);

		return res;
	}
};

class SkyBoxLoader : public AResourceLoader<SkyBox>, public Singleton<SkyBoxLoader>{
public:
	void create(SkyBox* sky){
		sky->create();
	}
};

typedef AResourceGroup<SkyBox> SkyBoxGroup;

}; // </wt>

#endif

