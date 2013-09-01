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
	LuaObject box(T& instance);

	String getErrorString();

	LuaStateOwner& getStateOwner();

	LuaObject getGlobals();

	template<class T>
	void expose(T& obj, const String& name);

private:
	LuaStateOwner mState;
}; // </State>



template<class T>
LuaPlus::LuaObject State::box(T& instance){
	if(instance.isBoxed()){
		return instance.getBoxedObject();
	}
	else{
		MetaInfo* mi = ((Object<T>*)(&instance))->getMetaInfo();

		if(mi->meta == NULL){
			mi->meta = new LuaObject;
			mi->meta->AssignNewTable(mState);
			instance.generateMetaTable();

			mi->meta->SetObject("__index", *mi->meta);
		}

	#if 0
		LuaObject luaObj = mState->BoxPointer(&instance);
	#else
		LuaObject luaObj = newTable();
		luaObj.SetLightUserData("__object", &instance);
		
	#endif
		luaObj.SetMetaTable(*mi->meta);

		instance.setBoxedObject(luaObj);

		return luaObj;
	}
}

template<class T>
void State::expose(T& obj, const String& name){
	getGlobals().Set(name.c_str(), static_cast<void*>(&obj));
}

}; // </lua>

}; // </wt>

#define TD_TRACE_TAG ""

#endif // </WT_LUASTATE_H>