#include "wt/stdafx.h"

#include "wt/Lua.h"
#include "wt/Defines.h"
#include "wt/Color.h"
#include "wt/Exception.h"

#define TD_TRACE_TAG "Lua"

namespace wt{

namespace Lua{


using namespace LuaPlus;


// Uint32
bool luaConv(const LuaObject& src, Uint32& dst){
	if(src.IsInteger()){
		dst = src.GetInteger();
		return true;
	}
	else{
		return false;
	}
}


// bool
bool luaConv(const LuaObject& src, bool& dst){
	if(src.IsBoolean()){
		dst = src.GetBoolean();
		return true;
	}
	else{
		return false;
	}
}

bool luaConv(const LuaObject& src, float& dst){
	if(src.IsNumber()){
		dst = src.GetNumber();
		return true;
	}
	else{
		return false;
	}
}

// vec 2
bool luaConv(const LuaObject& src, glm::vec2& dst){
	if(!luaConv(src.Get(1), dst.x) || !luaConv(src.Get(2), dst.y)){
		return false;
	}
	else{
		return true;
	}
}

bool luaConv(const glm::vec2& src, LuaObject& dst){
	if(!dst.IsTable()){
		return false;
	}
	else{
		dst.Set(1, src.x);
		dst.Set(2, src.y);
		return true;
	}
}


// vec 3
bool luaConv(const glm::vec3& src, LuaObject& dst){
	if(!dst.IsTable()){
		return false;
	}
	else{
		dst.Set(1, src.x);
		dst.Set(2, src.y);
		dst.Set(3, src.z);

		return true;
	}
}

bool luaConv(const LuaObject& src, glm::vec3& dst){
	if(!src.IsTable() || !luaConv(src.Get(1), dst.x) || !luaConv(src.Get(2), dst.y) || !luaConv(src.Get(3), dst.z)){
		return false;
	}
	else{
		return true;
	}
}

// quat
bool luaConv(const glm::quat& src, LuaObject& dst){
	if(!dst.IsTable()){
		return false;
	}
	else{
		dst.Set(1, src.x);
		dst.Set(2, src.y);
		dst.Set(3, src.z);
		dst.Set(4, src.w);

		return true;
	}
}

bool luaConv(const LuaObject& src, glm::quat& dst){
	if(!luaConv(src.Get(1), dst.x) || 
		!luaConv(src.Get(2), dst.y) || 
		!luaConv(src.Get(3), dst.z) ||
		!luaConv(src.Get(4), dst.w)){
		return false;
	}
	else{
		return true;
	}
}

// Color
bool luaConv(const Color& src, LuaObject& dst){
	if(!dst.IsTable()){
		return false;
	}
	else{
		dst.Set(1, src.mRed);
		dst.Set(2, src.mGreen);
		dst.Set(3, src.mBlue);
		dst.Set(4, src.mAlpha);

		return true;
	}
}

bool luaConv(const LuaObject& src, Color& dst){
	if(!luaConv(src.Get(1), dst.mRed) || 
		!luaConv(src.Get(2), dst.mGreen) || 
		!luaConv(src.Get(3), dst.mBlue) ||
		!luaConv(src.Get(4), dst.mAlpha)){
		return false;
	}
	else{
		return true;
	}
}

// integer
bool luaConv(const LuaObject& src, int& dst){
	if(!src.IsInteger()){
		return false;
	}
	else{
		dst = src.GetInteger();
		return true;
	}
}

// String
bool luaConv(const LuaObject& src, String& dst){
	if(!src.IsString()){
		return false;
	}
	else{
		dst = src.GetString();
		return true;
	}
}

std::ostream& operator<<(std::ostream& stream, LuaObject& obj){
	int type = obj.Type();

	switch(type){

	case LUA_TNUMBER:
		if(obj.IsNumber()){
			stream << obj.ToNumber();
		}
		else{
			stream << obj.ToInteger();
		}
		break;

	case LUA_TSTRING:
		stream << "\"" << obj.ToString() << "\"";
		break;

	case LUA_TTABLE:
		stream << "{";

		for(LuaTableIterator iter(obj); iter; iter.Next()){
			LuaObject& key = iter.GetKey();
			LuaObject& value = iter.GetValue();

			if(key.IsInteger()){ // array
				stream << value << ", ";
			}
			else if(key.IsString()){
				stream << "[" << key << "] = " << value << ",";
			}
			else{
				WT_THROW("Keys of type \"%s\" not yet supported for serialization", key.TypeName());
			}
		}

		stream << "}\n";
		break;
	}

	return stream;
}

std::ostream& serialize(LuaObject& obj, std::ostream& stream){
	return stream << obj;
}

void serializeTable(LuaObject& table, std::ostream& stream, Uint32 depth){
	WT_UNUSED(depth);
	serialize(table, stream);
}

}; // </Lua

}; // </wt>
