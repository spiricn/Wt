#include "wt/stdafx.h"
#include "wt/AFileSystem.h"
#include "wt/Lua.h"
#include "wt/lua/State.h"

namespace wt
{

using namespace lua;

void AFileSystem::Desc::serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const{
	switch(type){
	case eTYPE_LOCAL:
		dst.Set("type", "eTYPE_LOCAL");
		break;
	default:
		WT_THROW("Unsupported file system %d", type);
	}

	// Dir fs
	if(type == eTYPE_LOCAL){
		LuaObject table = luaState->newTable();
		table.Set("root", dir.root.c_str());
		dst.Set("dir", table);
	}
}

void AFileSystem::Desc::deserialize(lua::State* luaState, const LuaPlus::LuaObject& src){
	String typeStr;

	if(!luaConv(src.Get("type"), typeStr)){
		WT_THROW("Unsupported file system");
	}

	if(typeStr.compare("eTYPE_LOCAL") == 0){
		type = eTYPE_LOCAL;

		LuaObject table = src.Get("dir");

		luaConv(table.Get("root"), dir.root);
	}
	else{
		WT_THROW("Unsupported file system %d", type);
	}
}

} // </wt>
