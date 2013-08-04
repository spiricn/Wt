#ifndef WT_LUASTATE_H
#define WT_LUASTATE_H

#include <LuaPlus.h>

#include "wt/Sp.h"
#include "wt/lua/Script.h"
#include "wt/lua/Object.h"

#define TD_TRACE_TAG "LuaState"

namespace wt{

using namespace LuaPlus;

namespace lua{

typedef Sp<Script> ScriptPtr;

class State{
public:

	/*mGlobalState->DoString(
			"package.path = package.path .. \";d:/Documents/Programiranje/C++/Projekti/wt/assets/scripts/?.lua\"\n"
			);*/

	State(bool registerLogging=true);

	ScriptPtr createScript(const char* scriptFile);

	void assignTable(LuaObject& obj);

	LuaObject newTable();

	template<class T>
	LuaObject box(T& instance){
		MetaInfo* mi = instance.getMetaInfo();

		if(mi->meta == NULL){
			mi->meta = new LuaObject;
			mi->meta->AssignNewTable(mState);
			instance.generateMetaTable();

			mi->meta->SetObject("__index", *mi->meta);
		}

		LuaObject luaObj = mState->BoxPointer(&instance);
		luaObj.SetMetaTable(*mi->meta);

		return luaObj;
	}

	String getErrorString();

	LuaStateOwner& getStateOwner();

	LuaObject getGlobals();

private:
	LuaStateOwner mState;
}; // </State>


}; // </lua>

}; // </wt>

#define TD_TRACE_TAG ""

#endif // </WT_LUASTATE_H>