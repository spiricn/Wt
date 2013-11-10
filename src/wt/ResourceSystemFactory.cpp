#include "wt/stdafx.h"

#include "wt/ResourceSystemFactory.h"

#include "wt/Assets.h"

namespace wt
{

AResourceSystem* ResourceSystemFactory::create(const AResourceSystem::Desc& desc){
	Assets* res = new Assets;

	res->setFileSystem(desc.fileSystemType, desc.fileSystemUri);
	
	return res;
}

} // </wt>