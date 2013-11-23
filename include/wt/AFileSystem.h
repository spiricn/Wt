#pragma once
#ifndef WT_AFILESYSTEM_H
#define WT_AFILESYSTEM_H

#include "wt/Sp.h"
#include "wt/AIOStream.h"

namespace wt{

typedef Sp<AIOStream> StreamPtr;

class AFileSystem{
public:
	enum FileSystemType{
		eFS_INVALID = -1,

		eFS_DIR = 0,
		eFS_ZIP,
		eFS_REMOTE
	}; //</FileSystemType>

	struct Desc{
		FileSystemType type;

		struct DirFs{
			String root;

			DirFs() : root("."){
			}
		} dir;

		struct ZipFs{
			String path;
			String root;

			ZipFs() : path(""), root("."){
			}
		} zip;

		struct RemoteFs{
			String serverAddress;
			int32_t serverPort;
			String root;

			RemoteFs() : serverAddress("127.0.0.1"), serverPort(8420), root("."){
			}
		} remote;

		Desc() : type(eFS_INVALID){
		}

	}; // </Desc>

	virtual ~AFileSystem(){
	}

	virtual String getRoot() const = 0;

	virtual StreamPtr open(const String& uri, AIOStream::Mode mode) = 0;
}; // </AFileSystem>

}; // </wt>

#endif // </WT_AFILESYSTEM_H>