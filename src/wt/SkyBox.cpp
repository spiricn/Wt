#include "wt/stdafx.h"
#include "wt/SkyBox.h"

#define TD_TRACE_TAG "SkyBox"

namespace wt
{

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

	mDesc.serialize(luaState, dst);
}

void SkyBox::deserialize(lua::State* luaState, const LuaPlus::LuaObject& table){
	AResource::deserialize(luaState, table);

	mDesc.deserialize(luaState, table);
}

gl::Batch& SkyBox::getBatch(){
	return mBatch;
}

math::Transform& SkyBox::getTransform(){
		return mTransform;
}

void SkyBox::setImages(Image* posX, Image* negX, Image* posY, Image* negY, Image* posZ, Image* negZ){
	mDesc.map.pos_x = posX->getPath();
	mDesc.map.neg_x = negX->getPath();

	mDesc.map.pos_y = posY->getPath();
	mDesc.map.neg_y = negY->getPath();

	mDesc.map.pos_z = posZ->getPath();
	mDesc.map.neg_z = negZ->getPath();
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
	#define __tmp_load_face(side, face) \
		do{\
			Image* image = getSideImage(side); \
			WT_ASSERT(image, "Side image missing for side %d", side); \
			glTexImage2D(face, 0, image->getNumComponents(), \
			image->getWidth(), image->getHeigth(), 0, image->getFormat(), GL_UNSIGNED_BYTE, image->getData()); \
		}while(0)
  
	__tmp_load_face(eSIDE_POS_X, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	__tmp_load_face(eSIDE_NEG_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);

	__tmp_load_face(eSIDE_POS_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	__tmp_load_face(eSIDE_NEG_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);

	__tmp_load_face(eSIDE_POS_Z, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	__tmp_load_face(eSIDE_NEG_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

	#undef __tmp_load_face
			
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

Image* SkyBox::getSideImage(Side side) const{
	String rsrcPath = "";

	switch(side){
	case eSIDE_POS_X:
		rsrcPath = mDesc.map.pos_x;
		break;

	case eSIDE_POS_Y:
		rsrcPath = mDesc.map.pos_y;
		break;

	case eSIDE_POS_Z:
		rsrcPath = mDesc.map.pos_z;
		break;

	case eSIDE_NEG_X:
		rsrcPath = mDesc.map.neg_x;
		break;

	case eSIDE_NEG_Y:
		rsrcPath = mDesc.map.neg_y;
		break;

	case eSIDE_NEG_Z:
		rsrcPath = mDesc.map.neg_z;
		break;

	default:
		WT_THROW("Invalid SkyBox side %d", side);
	}

	return getManager()->getResourceSystem()->getImageManager()->getFromPath(rsrcPath);
}

}; // </wt>
