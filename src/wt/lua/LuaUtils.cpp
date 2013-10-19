#include "wt/stdafx.h"

#include "wt/lua/LuaUtils.h"

#include <LuaPlus.h>


using namespace LuaPlus;
namespace wt{

namespace lua{

static const char* kDEFAULT_TAG = "LuaScript";

#define LUA_LOG(arg1, arg2, level) do{ td_logMessage( arg2.IsString() ? arg1.ToString() : kDEFAULT_TAG, level, arg2.IsString() ? arg2.ToString() : arg1.ToString() ); }while(0)

void logi(LuaObject arg1, LuaObject arg2){
	LUA_LOG(arg1, arg2, eTD_LEVEL_INFO);
}

void loge(LuaObject arg1, LuaObject arg2){
	LUA_LOG(arg1, arg2, eTD_LEVEL_ERROR);
}

void logd(LuaObject arg1, LuaObject arg2){
	LUA_LOG(arg1, arg2, eTD_LEVEL_DEBUG);
}

void logw(LuaObject arg1, LuaObject arg2){
	LUA_LOG(arg1, arg2, eTD_LEVEL_WARNING);
}

void logv(LuaObject arg1, LuaObject arg2){
	LUA_LOG(arg1, arg2, eTD_LEVEL_VERBOSE);
}

void log(LuaObject arg1, LuaObject arg2){
	logd(arg1, arg2);
}

}; // </lua>

}; // </wt>