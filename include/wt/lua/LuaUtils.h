#ifndef WT_LUAUTILS_H
#define WT_LUAUTILS_H

#include <LuaPlus.h>

namespace wt{

namespace lua{

using namespace LuaPlus;

void logi(LuaObject arg1, LuaObject arg2);

void loge(LuaObject arg1, LuaObject arg2);

void logd(LuaObject arg1, LuaObject arg2);

void logw(LuaObject arg1, LuaObject arg2);

void logv(LuaObject arg1, LuaObject arg2);

void log(LuaObject arg1, LuaObject arg2);

}; // </lua>

}; // </wt>

#endif // </WT_LUAUTILS_H>