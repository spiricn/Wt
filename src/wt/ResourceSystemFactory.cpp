#include "wt/stdafx.h"

#include "wt/ResourceSystemFactory.h"
#include "wt/RemoteFileSystemClient.h"
#include "wt/Assets.h"

namespace wt
{

AResourceSystem* ResourceSystemFactory::create(){
	return new Assets;
}

} // </wt>