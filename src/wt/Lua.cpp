#include "wt/stdafx.h"

#include "wt/Lua.h"
#include "wt/Defines.h"
#include "wt/Color.h"
#include "wt/Exception.h"

#define TD_TRACE_TAG "Lua"

namespace wt{

namespace lua{


using namespace LuaPlus;

bool luaConv(const LuaObject& src, void** userdata){
	if(src.IsTable()){
		LuaObject object = src.GetByName("__object");
		if(object.IsLightUserData()){
			*userdata = object.GetLightUserData();
			return true;
		}
	}

	return false;
}

// uint32_t
bool luaConv(const LuaObject& src, uint32_t& dst){
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
		dst.Set(1, src.red);
		dst.Set(2, src.green);
		dst.Set(3, src.blue);
		dst.Set(4, src.alpha);

		return true;
	}
}

bool luaConv(const LuaObject& src, Color& dst){
	if(!src.IsTable() || !luaConv(src.Get(1), dst.red) || 
		!luaConv(src.Get(2), dst.green) || 
		!luaConv(src.Get(3), dst.blue) ||
		!luaConv(src.Get(4), dst.alpha)){
		return false;
	}
	else{
		return true;
	}
}

// int32_t
bool luaConv(const LuaObject& src, int32_t& dst){
	if(!src.IsInteger()){
		return false;
	}
	else{
		dst = src.GetInteger();
		return true;
	}
}

// uint8_t
bool luaConv(const LuaObject& src, uint8_t& dst){
	if(!src.IsInteger()){
		return false;
	}
	else{
		dst = static_cast<uint8_t>(src.GetInteger());
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


void serialize(LuaObject& obj, AIOStream& stream){
	int type = obj.Type();

	switch(type){

	case LUA_TNUMBER:{
		// TODO there must be a better way of finding out wheter an object
		// is integer or float (LuaPlus does not seem to have a working one)
		double number = obj.ToNumber();

		if(fmod(number, 1.0) == 0.0){
			// Integer
			stream.print("%d", (int32_t)number);
		}
		else{
			// Number
			stream.print("%f", number);
		}
		break;
	}
	case LUA_TSTRING:
		stream.print("\"%s\"", obj.ToString()); 
		break;

	case LUA_TBOOLEAN:
		stream.print("%s", (obj.GetBoolean() ? "true" : "false"));
		break;

	case LUA_TNIL:
		stream.print("nil");
		break;

	case LUA_TTABLE:
		stream.print("{");

		for(LuaTableIterator iter(obj); iter; iter.Next()){
			LuaObject& key = iter.GetKey();
			LuaObject& value = iter.GetValue();

			if(key.IsInteger()){ // array
				serialize(value, stream);
				stream.print(", ");
			}
			else if(key.IsString()){
				stream.print("[");
				serialize(key, stream);

				stream.print("] = ");
				serialize(value, stream);

				stream.print(",");
			}
			else{
				WT_THROW("Keys of type \"%s\" not yet supported for serialization", key.TypeName());
			}
		}

		stream.print("}\n");
		break;
	}
}

void serializeTable(LuaObject& table, AIOStream& stream, uint32_t depth){
	WT_UNUSED(depth);
	serialize(table, stream);
}
void luaDoStream(LuaPlus::LuaStateOwner& state, AIOStream& stream, LuaPlus::LuaObject& fenv){
}

void doStream(LuaPlus::LuaStateOwner& state, AIOStream& stream, LuaPlus::LuaObject* fenv){
	WT_ASSERT(stream.isReadable(), "Stream not readable");

	char* bfr = new char[stream.getSize() + 1];
	memset(bfr, 0x00, stream.getSize() + 1);

	stream.read(bfr, stream.getSize());
	// TODO switch to using DoBuffer ?

	try{
		if(fenv){
			state->DoString(bfr, *fenv);
		}
		else{
			state->DoString(bfr);
		}
	}catch(...){
		// Prevent memory leakage in case of an exception
		delete[] bfr;
		throw;
	}

	delete[] bfr;
}

bool luaConv(const LuaObject& src, ATransformable& dst){
	// Position
	float px, py, pz;
	if(!luaConv(src.Get(1), px)) return false;
	if(!luaConv(src.Get(2), py)) return false;
	if(!luaConv(src.Get(3), pz)) return false;
	dst.setTranslation(glm::vec3(px, py, pz));

	// Scale
	float sx, sy, sz;
	if(!luaConv(src.Get(4), sx)) return false;
	if(!luaConv(src.Get(5), sy)) return false;
	if(!luaConv(src.Get(6), sz)) return false;
	dst.setScale(glm::vec3(sx, sy, sz));

	// Rotation
	float rx, ry, rz, rw;
	if(!luaConv(src.Get(7), rx)) return false;
	if(!luaConv(src.Get(8), ry)) return false;
	if(!luaConv(src.Get(9), rz)) return false;
	if(!luaConv(src.Get(10), rw)) return false;
	glm::quat rot;
	rot.x = rx;
	rot.y = ry;
	rot.z = rz;
	rot.w = rw;

	dst.setRotation(rot);

	return true;
}

bool luaConv(const ATransformable& src, LuaObject& dst){
	if(!dst.IsTable()){
		return false;
	}

	glm::vec3 pos, scale;
	glm::quat rot;

	src.decomposeTransform(pos, scale, rot);

	// Position
	dst.Set(1, pos.x);
	dst.Set(2, pos.y);
	dst.Set(3, pos.z);

	// Scale
	dst.Set(4, scale.x);
	dst.Set(5, scale.y);
	dst.Set(6, scale.z);

	// Rotation
	dst.Set(7, rot.x);
	dst.Set(8, rot.y);
	dst.Set(9, rot.z);
	dst.Set(10, rot.w);

	return true;
}

}; // </Lua

}; // </wt>
