#include "wt/stdafx.h"

#include "wt/FileSystemFactory.h"
#include "wt/LocalFileSystem.h"
#include "wt/RemoteFileSystemClient.h"
#include "wt/ZipFileSystem.h"

namespace wt
{

AFileSystem* FileSystemFactory::create(const AFileSystem::Desc& desc){
	if(desc.type == AFileSystem::eFS_DIR){
		return new LocalFileSystem(desc.dir.root);
	}
	else if(desc.type == AFileSystem::eFS_ZIP){
		return new ZipFileSystem(desc.zip.path, desc.zip.root);
	}
	else if(desc.type == AFileSystem::eFS_REMOTE){
		return new RemoteFileSystemClient(desc.remote.root, desc.remote.serverAddress, desc.remote.serverPort);
	}
	else{
		TRACEE("Invalid file system type; %d", desc.type);
		return NULL;
	}
}

} // </wt>
