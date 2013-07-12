#ifndef WT_LUASTATEMANAGER_H
#define WT_LUASTATEMANAGER_H


#include "wt/stdafx.h"


#include "wt/Log.h"
#include "wt/Exception.h"
#include "wt/Singleton.h"

#include "wt/Lua.h"

namespace wt{

using namespace LuaPlus;

class LuaStateManager;

class ALuaObject{
public:
	virtual void expose(LuaObject& meta)=0;
};

class LuaStateManager : public Singleton<LuaStateManager>{
private:
	static const char* TAG;

	LuaStateOwner mGlobalState;
	LuaObject mMetaTable;

public:
	LuaStateManager() : mGlobalState(true) {
		
		mGlobalState->DoString(
			"package.path = package.path .. \";d:/Documents/Programiranje/C++/Projekti/wt/assets/scripts/?.lua\"\n"
			);

		mMetaTable = mGlobalState->GetGlobals().CreateTable("LuaStateManagerMetaTable");
		mMetaTable.SetObject("__index", mMetaTable);
	
		mMetaTable.RegisterObjectDirect(
			"log", (LuaStateManager*)NULL, &LuaStateManager::logMessage);


		registerObject(this, mMetaTable, "LuaStateManager");
	}

	LuaObject exposeObject(ALuaObject* obj, const String& metaName){
		LuaObject meta = mGlobalState->GetGlobals().CreateTable(metaName.c_str());
		meta.SetObject("__index", meta);

		obj->expose(meta);

		return meta;
	}
	

	void errorCheck(bool condition){
		if(condition){
			LuaStackObject  top = mGlobalState->StackTop();
			if(top.IsString()){
				WT_EXCEPT(TAG,
					"Lua script error occured: \"%s\"", top.GetString());
			}
			else{
				WT_EXCEPT(TAG,
					"Lua script error occured");
			}
		}
	}

	void doString(const String& str){
		errorCheck( mGlobalState->DoString(str.c_str())==0?false:true );
	}

	LuaStateOwner& getGlobalState(){
		return mGlobalState;
	}

	LuaObject registerObject(void* instance, LuaObject meta, const String& name){
		LuaObject obj = mGlobalState->BoxPointer(instance);
		obj.SetMetaTable(meta);
		mGlobalState->GetGlobals().SetObject(name.c_str(), obj);
		return obj;
	}

	void init(const String& path){
		doFile(path);
	}

	void doFile(const String& path){
		int res = 1;
		try{
			res = mGlobalState->DoFile(path.c_str());
		}catch(LuaPlus::LuaException){
		}

		errorCheck( res==0?false:true );
		
	}

	void logMessage(LuaObject arg1, LuaObject arg2, LuaObject arg3){
		if(arg1.IsInteger() && arg2.IsString() && arg3.IsString()){
			// LEVEL, TAG, MESSAGE
			Log::log((Log::Level)arg1.ToInteger(), arg2.ToString(), arg3.ToString());
		}
		else if(arg1.IsString() && arg2.IsString()){
			// TAG, MESSAGE
			LOGD(arg1.ToString(), arg2.ToString());
		}
		else if(arg1.IsString()){
			// MESSAGE
			LOGD(TAG, arg1.ToString());
		}
	}

}; // </LuaStateManager>


}; // </wt>

#define LUA_NEW_TABLE(t) t.AssignNewTable( wt::LuaStateManager::getSingleton().getGlobalState() );

#endif
