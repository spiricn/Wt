#include "wt/stdafx.h"
#include "wt/DevilImageLoader.h"

namespace wt{

DevilImageLoader::DevilImageLoader() : mImageHandle(0){
	if(ilGetInteger(IL_VERSION_NUM) != IL_VERSION){
		WT_EXCEPT("DevilImageLoader", 
			"Invalid Devil version (DLL doesn't match the headers)");
	}

	ilInit();
	iluInit();

	mImageHandle = iluGenImage();
	ilBindImage(mImageHandle);
}

String DevilImageLoader::getErrorString(){
	ILenum error = ilGetError();
	String res;

	if(error != IL_NO_ERROR){
		return iluErrorString(error);
	}

	return res;
}

Image::Format convertFormat(int format){
	switch(format){
	case IL_RGB:
		return Image::RGB;
	case IL_BGR:
		return Image::BGR;
	case IL_RGBA:
		return Image::RGBA;
	case IL_BGRA:
		return Image::BGRA;
	default:
		WT_EXCEPT("DevilImageLoader",
			"Unsupported format %d", format);
	}
}

void DevilImageLoader::save(const String& path, Image* image){
	ILuint handle = ilGenImage();

	ilBindImage(handle);

	ILint format = image->getFormat();

	ilTexImage(image->getWidth(),
		image->getHeigth(),
		1,
		image->getNumComponents(),
		format,
		IL_UNSIGNED_BYTE,
		(void*)image->getData()
		);

	ilEnable(IL_FILE_OVERWRITE);

	ilSave(IL_TYPE_UNKNOWN, path.c_str());

	if(ilGetError()!=IL_NO_ERROR){
		WT_EXCEPT("DevilImageLoader", "Error saving image \"%s\"",
			path.c_str());
	}

	ilDeleteImage(handle);
}

void DevilImageLoader::load(const String& path, Image* image){
	ilBindImage(mImageHandle);

	if(ilLoadImage(path.c_str()) != IL_TRUE){
		WT_EXCEPT("DevilImageLoader", "Error loading image \"%s\" : %s",
			path.c_str(), getErrorString().c_str());
	}

	int w = ilGetInteger(IL_IMAGE_WIDTH);

	int h = ilGetInteger(IL_IMAGE_HEIGHT);

	ILubyte* data = ilGetData();

	Image::Format format =  (Image::Format)ilGetInteger(IL_IMAGE_FORMAT);

	int numComponents = ilGetInteger(IL_IMAGE_CHANNELS);

	image->setData(w, h, format, numComponents,
		data);
}

}; // </wt>
