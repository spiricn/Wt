#include "wt/stdafx.h"

#include "wt/lua/State.h"

#include "wt/lua/LuaUtils.h"

namespace wt{

namespace lua{

State::State(bool registerLogging) : mState(true /*init standard library*/){
	if(registerLogging){
		getGlobals().RegisterDirect("LOGI", lua::logi);
		getGlobals().RegisterDirect("LOGE", lua::loge);
		getGlobals().RegisterDirect("LOGD", lua::logd);
		getGlobals().RegisterDirect("LOGV", lua::logv);
		getGlobals().RegisterDirect("LOGW", lua::logw);
		getGlobals().RegisterDirect("LOG", lua::log);
	}
}

ScriptPtr State::createScript(const char* scriptFile){
	ScriptPtr script = new Script(this);
	script->getState().AssignNewTable(mState);

	script->getState().Set("__index", mState->GetGlobal("_G"));
	script->getState().SetMetaTable(script->getState());

	if(mState->DoFile(scriptFile, script->getState()) != 0){
		WT_THROW("Error executing script \"%s\"", getErrorString().c_str());
	}

	return script;
}

void State::assignTable(LuaObject& obj){
	obj.AssignNewTable(mState);
}

LuaObject State::newTable(){
	LuaObject obj;
	obj.AssignNewTable(mState);
	return obj;
}

LuaStateOwner& State::getStateOwner(){
	return mState;
}

LuaObject State::getGlobals(){
	return mState->GetGlobals();
}

String State::getErrorString(){
	LuaStackObject top = mState->StackTop();
	if(top.IsString()){
		return top.GetString();
	}
	else{
		return "";
	}
}

}; // </lua>

}; // </wt>


