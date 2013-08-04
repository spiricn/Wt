#include "wt/stdafx.h"
#include "wt/SkyBox.h"
#include "wt/ImageManager.h"

#define TD_TRACE_TAG "SkyBox"

namespace wt{

SkyBox::SkyBox(AResourceManager<SkyBox>* manager, ResourceHandle handle, const std::string& name) : AResource(manager, handle, name){
	mTransform.setScale(1000, 1000, 1000);
}

SkyBox::~SkyBox(){
	mBatch.destroy();
	glDeleteTextures(1, &mTexture);
}

void SkyBox::bind(){
	glBindTexture(GL_TEXTURE_CUBE_MAP, mTexture);
}

void SkyBox::serialize(lua::State* luaState, LuaPlus::LuaObject& dst){
	AResource::serialize(luaState, dst);

	lua::LuaObject map = luaState->newTable();

	dst.Set("map", map);

	map.Set("pos_x", mPosX.getPath().c_str());
	map.Set("neg_x", mNegX.getPath().c_str());

	map.Set("pos_y", mPosY.getPath().c_str());
	map.Set("neg_y", mNegY.getPath().c_str());

	map.Set("pos_z", mPosZ.getPath().c_str());
	map.Set("neg_z", mNegZ.getPath().c_str());
}

void SkyBox::deserialize(lua::State* luaState, const LuaPlus::LuaObject& table){
	setImages(
		mImageManager->getFromPath(table.Get("map").Get("pos_x").ToString()),
		mImageManager->getFromPath(table.Get("map").Get("neg_x").ToString()),
		mImageManager->getFromPath(table.Get("map").Get("pos_y").ToString()),
		mImageManager->getFromPath(table.Get("map").Get("neg_y").ToString()),
		mImageManager->getFromPath(table.Get("map").Get("pos_z").ToString()),
		mImageManager->getFromPath(table.Get("map").Get("neg_z").ToString())
		);
}

gl::Batch& SkyBox::getBatch(){
	return mBatch;
}

void SkyBox::setImages(Image* posX, Image* negX,
		Image* posY, Image* negY, 
		Image* posZ, Image* negZ){
	mPosX = posX->getLink();
	mNegX = negX->getLink();

	mPosY = posY->getLink();
	mNegY = negY->getLink();

	mPosZ = posZ->getLink();
	mNegZ = negZ->getLink();
}

void SkyBox::create(){
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mTexture);
        
	// Set up texture maps        
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,		GL_CLAMP_TO_EDGE);       
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
	// Temporary macro for loading textures
	#ifndef LoadFace
	#define LoadFace(link, face) \
		do{\
			Image* image = (Image*)link.getPtr(); \
			glTexImage2D(face, 0, image->getNumComponents(), \
			image->getWidth(), image->getHeigth(), 0, image->getFormat(), GL_UNSIGNED_BYTE, image->getData()); \
		}while(0)
	#endif
  
	LoadFace(mPosX, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	LoadFace(mNegX, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);

	LoadFace(mPosY, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	LoadFace(mNegY, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);

	LoadFace(mPosZ, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	LoadFace(mNegZ, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

	#undef LoadFace
			
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// Counter-clockwise wound for backface culling
	static const Vertex vertices[8] = {
		//0
		{-1,-1,-1},
		//1
		{1,-1,-1},
		//2
		{1,-1,1},
		//3
		{-1,-1,1},

		//4
		{-1,1,-1},
		//5
		{1,1,-1},
		//6
		{1,1,1},
		//7
		{-1,1,1}};

	static const GLubyte indices[24] = {
		//neg_y
		0,3,2,1,
		//pos_y
		4,5,6,7,
		//neg_z
		0,1,5,4,
		//pos_z
		3,7,6,2,
		//pos_x
		1,2,6,5,
		//neg_x
		0,4,7,3};

	mBatch.create(
		GL_QUADS,
		vertices, 8, sizeof(Vertex),
		indices, 24, sizeof(GLubyte),
		GL_UNSIGNED_BYTE
	);

	mBatch.setVertexAttribute(0, 3, GL_FLOAT, 0);

}

}; // </wt>
