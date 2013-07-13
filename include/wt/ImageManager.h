#ifndef WT_IMAGEMANAGER_H
#define WT_IMAGEMANAGER_H


#include "wt/stdafx.h"

#include "wt/Image.h"
#include "wt/Singleton.h"
#include "wt/AResourceManager.h"
#include "wt/AResourceSystem.h"

namespace wt{

typedef AResourceGroup<Image> ImageGroup;

class ImageManager : public AResourceManager<Image>{
public:
	ImageManager(AResourceSystem* assets) : AResourceManager(assets){
	}
};

}; // </wt>

#endif
