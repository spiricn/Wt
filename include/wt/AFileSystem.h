#pragma once
#ifndef WT_AFILESYSTEM_H
#define WT_AFILESYSTEM_H

#include "wt/Sp.h"
#include "wt/AIOStream.h"
#include "wt/ASerializable.h"

namespace wt
{

typedef Sp<AIOStream> StreamPtr;

class AFileSystem{
public:
	enum Type{
		eTYPE_INVALID = -1,

		eTYPE_LOCAL = 0,
		eTYPE_ZIP,
		eTYPE_REMOTE,

		eTYPE_MAX
	}; //</Type>

	struct Desc : public lua::ASerializable{
		Type type;

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

		void serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const;

		void deserialize(lua::State* luaState, const LuaPlus::LuaObject& src);

		Desc() : type(eTYPE_INVALID){
		}

	}; // </Desc>

	virtual ~AFileSystem(){
	}

	virtual String getRoot() const = 0;

	virtual StreamPtr open(const String& uri, AIOStream::Mode mode) = 0;
}; // </AFileSystem>

}; // </wt>

#endif // </WT_AFILESYSTEM_H>