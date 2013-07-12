#ifndef WT_DEVILIMAGELOADER_H
#define WT_DEVILIMAGELOADER_H


#include "wt/stdafx.h"

#include "wt/Image.h"

#include "wt/AResourceLoader.h"
#include "wt/Singleton.h"


namespace wt{

class DevilImageLoader : public Singleton<DevilImageLoader>, public AResourceLoader<Image>{
private:
	ILuint mImageHandle;

	void initDevil();

	static String getErrorString();

	static Image::Format convertFormat(int format);

public:

	DevilImageLoader();
	
	void save(const String& path, Image* image);

	void load(const String& path, Image* image);

}; // </DevilImageLoader>

}; // </wt>

#endif
