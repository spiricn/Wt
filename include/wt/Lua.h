#ifndef WT_LUA_H
#define WT_LUA_H


#include "wt/stdafx.h"

#include "wt/AIOStream.h"
#include "wt/Transform.h"
#include "wt/AIOStream.h"
#include "wt/Utils.h"
#include "wt/ATransformable.h"

namespace wt{

namespace lua{
	
using namespace LuaPlus;


// user object
bool luaConv(const LuaObject& src, void** userdata);

// bool
bool luaConv(const LuaObject& src, bool& dst);

// float
bool luaConv(const LuaObject& src, float& dst);

// uint32_t
bool luaConv(const LuaObject& src, uint32_t& dst);

// int32_t
bool luaConv(const LuaObject& src, int32_t& dst);

// uint8_t
bool luaConv(const LuaObject& src, uint8_t& dst);

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

void serializeTable(LuaObject& table, AIOStream& stream, uint32_t depth=0);

void doStream(LuaPlus::LuaStateOwner& state, AIOStream& stream, LuaPlus::LuaObject* fenv=NULL);

// ATransformable
bool luaConv(const LuaObject& src, ATransformable& dst);

bool luaConv(const ATransformable& src, LuaObject& dst);

}; // </Lua

}; // </wt>

#endif
