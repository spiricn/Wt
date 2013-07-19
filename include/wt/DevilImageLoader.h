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
	

	void save(const String& path, const Image* image);

	void save(AIOStream* stream, Image* image, int format);

	void save(AIOStream* stream, Image* image);

	void load(AIOStream* stream, Image* image);

}; // </DevilImageLoader>

}; // </wt>

#endif
