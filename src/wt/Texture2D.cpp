#include "wt/stdafx.h"

#include "wt/Texture2D.h"
#include "wt/GLTrace.h"

#define TD_TRACE_TAG "Texture2D"

namespace wt{

void Texture2D::depthDump(Texture2D* src, const char* dst){
	src->bind();

	// Depth dump
	Buffer<float> depth;
	depth.create(src->mWidth*src->mHeight);
	gl( GetTexImage(src->getType(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, (GLvoid*)depth.getData()) );


	Buffer<uint8_t> rgb(src->mWidth*src->mHeight*3);

	for(uint32_t i=0; i<depth.getCapacity(); i++){
		float n = 1.0; // camera z near
		float f = 100.0; // camera z far
		float z = depth[i];
		z = (2.0 * n) / (f + n - z * (f - n));
		z *= 255;

		rgb[i*3 + 0] = z;
		rgb[i*3 + 1] = z;
		rgb[i*3 + 2] = z;
	}

	Image img;
	img.setData(src->mWidth, src->mHeight, Image::RGB, 3, (unsigned char*)rgb.getData());

	FileIOStream stream;
	stream.open(dst, AIOStream::eMODE_WRITE);

	DevilImageLoader::getSingleton().save(&stream,
		&img);

}


Color Texture2D::sample(float s, float t) const{
	Color res;
	
	GLint internalFormat, width, height;
	gl( GetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPONENTS, &internalFormat) ); // get internal format type of GL texture
	gl( GetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width) ); // get width of GL texture
	gl( GetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height) ); // get height of GL texture


	GLint bpp = 0;
	switch(internalFormat){
	case GL_RGB:
		bpp = 3;
		break;
	case GL_RGBA8:
	case GL_RGBA:
		bpp = 4;
		break;
	default:
		WT_THROW("Unsupported format");
		break;
	}

	GLubyte* pixels = new GLubyte[width*height*bpp];
	gl( GetTexImage(getType(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)pixels) );

	// The first element corresponds to the lower left corner of the texture image.
	// Subsequent elements progress left-to-right through the remaining texels in the lowest row of the texture image,
	// and then in successively higher rows of the texture image. The final element corresponds to the upper right corner of the texture image
	//
	//   - width - 
	// -> -> -> -> end
	// -> -> -> -> ->    | height
	// start -> -> ->

	
	// TODO handle different min/mag filters, wrapping etc..
	s = glm::clamp(s, 0.0f, 1.0f);
	t = glm::clamp(t, 0.0f, 1.0f);

	const uint32_t x = mWidth*s;
	const uint32_t y = mHeight*t;

	GLubyte* rgb = &pixels[(y * mWidth + x) * bpp];
	
	if(bpp >= 1){
		res.red = *(rgb + 0);

		if(bpp >= 2){
			res.green = *(rgb + 1);

			if(bpp >= 3){
				res.blue = *(rgb + 2);
			}

			if(bpp >= 4){
				res.alpha = *(rgb + 3);
			}
		}
	}

	delete[] pixels;

	return res;
}

void Texture2D::dump(AIOStream& stream){
	bind();

#if 0
	// Depth dump
	// TODO do this properly
	Buffer<float> depth;
	depth.create(mWidth*mHeight);
	gl( GetTexImage(getType(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, (GLvoid*)depth.getData()) );


	Buffer<uint8_t> rgb(mWidth*mHeight*3);

	for(uint32_t i=0; i<depth.getCapacity(); sa){
		float n = 1.0; // camera z near
		float f = 100.0; // camera z far
		float z = depth[i];
		z = (2.0 * n) / (f + n - z * (f - n));
		z *= 255;

		rgb[i*3 + 0] = z;
		rgb[i*3 + 1] = z;
		rgb[i*3 + 2] = z;
	}

	Image img;
	img.setData(mWidth, mHeight, Image::RGB, 3, (unsigned char*)rgb.getData());

	DevilImageLoader::getSingleton().save(&stream,
		&img);

#else
	Buffer<GLubyte> rgb(mWidth*mHeight*3);
	gl( GetTexImage(getType(), 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)rgb.getData()) );

	Image img;
	img.setData(mWidth, mHeight, Image::RGB, 3, (unsigned char*)rgb.getData());

	DevilImageLoader::getSingleton().save(&stream,
		&img);
#endif
}

void Texture2D::generateMipmap(){
	bind();
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture2D::dump(const String& path){
	// TODO Textrue2D should acquire this stream from elsewhere
	FileIOStream stream(path, AIOStream::eMODE_WRITE);

	dump(stream);

	stream.close();
}

void Texture2D::setData(GLint width, GLint height,
	GLenum format, GLenum internalFormat, const GLbyte* data, GLenum type, bool mmap){

	if(!isCreated()){
		WT_THROW("Must create texture before setting data (%s)", getName().c_str());
	}

	mFormat = format;
	mInternalFormat = internalFormat;
	mWidth = width;
	mHeight = height;

	bind();

	glTexImage2D(getType(), 0, internalFormat, mWidth, mHeight,
		0, mFormat, type, data);

	if(mmap && getType() != eTYPE_RECT){
		glGenerateMipmap(getType());
	}
}

void Texture2D::setData(Image* image){
	if(!isCreated()){
		WT_THROW("Must create texture before setting data (%s)", getName().c_str());
	}

	mFormat = image->getFormat();
	mInternalFormat = image->getNumComponents();
	mWidth = image->getWidth();
	mHeight = image->getHeigth();

	bind();

	glTexImage2D(getType(), 0, mInternalFormat, mWidth, mHeight,
		0, mFormat, GL_UNSIGNED_BYTE, image->getData());

	if(getType() != eTYPE_RECT){
		glGenerateMipmap(getType());
	}
}

Texture2D::Texture2D(AResourceManager<Texture2D>* manager, ResourceHandle handle, const String& name, Type type) : AResource(manager, handle, name), Texture(type), mWidth(0),
		mHeight(0), mFormat(0), mInternalFormat(0){
}

void Texture2D::copy(Texture2D* src, Texture2D* dst){
	dst->create();

	Buffer<uint8_t> p;
	uint32_t w=src->getWidth();
	uint32_t h=src->getHeigth();
	p.create(w*h*3);

	src->bind();
	glGetTexImage(src->getType(), 0, Image::RGB, GL_UNSIGNED_BYTE, (GLvoid*)p.getData());

	dst->setData(w, h, Image::RGB, Image::RGB, (const GLbyte*)p.getData(), GL_UNSIGNED_BYTE, false);
}

void Texture2D::setSubData(uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height,
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

GLuint Texture2D::getWidth() const{
	return mWidth;
}

GLuint Texture2D::getHeigth() const{
	return mHeight;
}

} // </wt>
