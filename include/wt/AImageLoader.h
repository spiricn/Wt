#ifndef WT_AIMAGELOADER_H
#define WT_AIMAGELOADER_H



#include "wt/Image.h"

namespace wt{

class AImageLoader{
public:
	virtual void save(const String& path, Image* image)=0;
	virtual void load(const String& path, Image* image)=0;
}; // </AImageLoader>

}; // </wt>

#endif
