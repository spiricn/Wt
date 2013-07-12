#ifndef WT_LUA_H
#define WT_LUA_H


#include "wt/stdafx.h"


#include "wt/Utils.h"

namespace wt{

namespace Lua{


using namespace LuaPlus;


// bool
bool luaConv(const LuaObject& src, bool& dst);

// float
bool luaConv(const LuaObject& src, float& dst);

// Uint32
bool luaConv(const LuaObject& src, Uint32& dst);

// integer
bool luaConv(const LuaObject& src, int& dst);

// String
bool luaConv(const LuaObject& src, String& dst);

// vec 2
bool luaConv(const LuaObject& src, glm::vec2& dst);

bool luaConv(const glm::vec2& src, LuaObject& dst);


// vec 3
bool luaConv(const glm::vec3& src, LuaObject& dst);

bool luaConv(const LuaObject& src, glm::vec3& dst);

// quat
bool luaConv(const glm::quat& src, LuaObject& dst);

bool luaConv(const LuaObject& src, glm::quat& dst);


// Color
bool luaConv(const Color& src, LuaObject& dst);

bool luaConv(const LuaObject& src, Color& dst);

void serializeTable(LuaObject& table, std::ostream& stream, Uint32 depth=0);

}; // </Lua

}; // </wt>

#endif