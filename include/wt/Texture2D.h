#ifndef WT_TEXTURE2D_H
#define WT_TEXTURE2D_H


#include "wt/Texture.h"
#include "wt/Image.h"
#include "wt/ImageManager.h"
#include "wt/AResource.h"
#include "wt/AResourceManager.h"
#include <td/td.h>
#include "wt/Singleton.h"
#include "wt/Buffer.h"
#include "wt/DevilImageLoader.h"

namespace wt{


class TextureLoader;

class Texture2D : public AResource<Texture2D>, public Texture{
friend class TextureLoader;
friend class TextureManager;

public:
	enum Type{
		eTEXTURE_2D = GL_TEXTURE_2D,
		eRECT_TEXTURE = GL_TEXTURE_RECTANGLE,
	};

protected:
	uint32_t mWidth, mHeight, mInternalFormat;
	GLenum mFormat;
	ImageLink mImage;
	ImageManager* mImageManager;

	void setImageManager(ImageManager* imageManager){
		mImageManager = imageManager;
	}

public:
	Texture2D(AResourceManager<Texture2D>* manager=NULL, ResourceHandle handle=0, const std::string& name="", Type type=eTEXTURE_2D) :
	  AResource(manager, handle, name), Texture(type), mWidth(0),
		  mHeight(0), mFormat(0), mInternalFormat(0){
	}

	void setData(GLint width, GLint height, GLenum format=GL_RGB,
		GLenum internalFormat=GL_RGB, const GLbyte* data=0, GLenum type=GL_UNSIGNED_BYTE, bool mmap=true);

	void setData(Image* image);

	ImageLink getImage(){
		return mImage;
	}

	void generateMipmap();

	void dump(AIOStream& stream);

	void dump(const String& path);

	static void copy(Texture2D* src, Texture2D* dst){

		dst->create();

		Buffer<uint8_t> p;
		uint32_t w=src->getWidth();
		uint32_t h=src->getHeigth();
		p.create(w*h*3);

		src->bind();
		glGetTexImage(src->getType(), 0, Image::RGB, GL_UNSIGNED_BYTE, (GLvoid*)p.getData());

		dst->setData(w, h, Image::RGB, Image::RGB, (const GLbyte*)p.getData(), GL_UNSIGNED_BYTE, false);
		
	}


	void setSubData(uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height,
		void* data){

		if(xOffset+width > mWidth || yOffset+height > mHeight){
			WT_THROW("Texture sub data dimensions out of bounds by {%d, %d}",
				mWidth-(xOffset+width), mHeight-(yOffset+height)
				);
		}

		bind();
		glTexSubImage2D(GL_TEXTURE_2D,
			0, xOffset, yOffset, width, height, mInternalFormat, GL_UNSIGNED_BYTE, data);
	}

	GLuint getWidth() const{
		return mWidth;
	}

	GLuint getHeigth() const{
		return mHeight;
	}

	/* Serialization */

	void serialize(lua::State* luaState, LuaPlus::LuaObject& dst){
		AResource::serialize(luaState, dst);

		if(!mUri.empty()){
			dst.Set("uri", mUri.c_str());
		}
		else{
			dst.Set("image", mImage.getPath().c_str());
		}

	}

	void deserialize(lua::State* luaState, const LuaPlus::LuaObject& table){
		if(table.Get("uri").IsString()){
			AResource::deserialize(luaState, table);
		}
		else{
			const char* path = table.Get("image").ToString();
			
			Image* image = mImageManager->getFromPath(path);

			if(image==NULL){
				WT_THROW("Unable to deserialize texture, image is missing \"%s\"", path);
			}

			mImage = ImageLink(image, path);
		}

	}

}; // </Texture2D>
 

class TextureLoader : public Singleton<TextureLoader>, public AResourceLoader<Texture2D>{
public:
	void load(AIOStream* stream, Texture2D* tex){
		// TODO terrible hack
		
		tex->create();

		if(tex->getImage().getPtr()){
			tex->setData(
				static_cast<Image*>(tex->getImage().getPtr())
			);
		}
		else{
			Image tmp;
			DevilImageLoader::getSingleton().load(stream, &tmp);

			tex->setData(&tmp);
		}
	}

	void save(AIOStream* /*stream*/, Texture2D* /*src*/){
	}

	void create(Texture2D* /*tex*/){
	}
}; // </TextureLoader>

class TextureManager : public AResourceManager<Texture2D>{
public:
	TextureManager(AResourceSystem* assets) : AResourceManager(assets){
	}

	Texture2D* create(const String& name){
		Texture2D* res = AResourceManager<Texture2D>::create(name);

		res->setImageManager( (ImageManager*)getResourceSystem()->getImageManager() );

		return res;
	}
};

typedef AResourceGroup<Texture2D> TextureGroup;

}; // </Wt>

#endif