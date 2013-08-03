#ifndef WT_LUASTATE_H
#define WT_LUASTATE_H

#include <LuaPlus.h>

#include "wt/Sp.h"
#include "wt/lua/Script.h"
#include "wt/lua/Object.h"

namespace wt{

namespace lua{

using namespace LuaPlus;


typedef Sp<Script> ScriptPtr;

class State{
public:
	State() : mState(true /*init standard library*/){
	}

	ScriptPtr createScript(const char* scriptFile){
		ScriptPtr script = new Script(this);
		script->getState().AssignNewTable(mState);

		script->getState().Set("__index", mState->GetGlobal("_G"));
		script->getState().SetMetaTable(script->getState());

		mState->DoFile(scriptFile, script->getState());

		return script;
	}

	void assignTable(LuaObject& obj){
		obj.AssignNewTable(mState);
	}

	template<class T>
	LuaObject box(Object<T>& instance, const char* name){
		LuaObject obj = box(instance);
		mState->GetGlobals().Set(name, obj);
	}

	template<class T>
	LuaObject box(Object<T>& instance){
		MetaInfo* mi = instance.getMetaInfo();

		if(mi->meta == NULL){
			mi->meta = new LuaObject;
			mi->meta->AssignNewTable(mState);
			instance.generateMetaTable(*mi->meta);
			mi->meta->SetObject("__index", *mi->meta);
		}

		LuaObject obj = mState->BoxPointer(&instance);
		obj.SetMetaTable(*mi->meta);

		return obj;
	}

	LuaStateOwner& operator->(){
		return mState;
	}

private:
	LuaStateOwner mState;
}; // </State>


}; // </lua>

}; // </wt>


#endif // </WT_LUASTATE_H>