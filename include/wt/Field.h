/**
 * @file Field.h
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */
 
#ifndef WT_FIELD_H
#define WT_FIELD_H

#include "wt/lua/State.h"
#include "wt/Lua.h"

namespace wt
{

class ISerializator{
public:
	virtual void deserialize(lua::State* state, LuaObject table) = 0;

	virtual void serialize(lua::State* state, LuaObject table) = 0;

	virtual ~ISerializator(){
	}
}; // </ISerializator>


class IField{
public:
	virtual void serialize(lua::State* state, void* holder, LuaObject table) = 0;

	virtual void deserialize(lua::State* state, void* holder, LuaObject table) = 0;

	virtual ~IField(){
	}
}; // </IField>

template<class ValueType, class HolderType>
class Field : public IField{
public:
	Field(const String& name, uint32_t offset) : mName(name), mOffset(offset){
	}

	void serialize(lua::State* state, void* holder, LuaObject table){
		WT_ASSERT(table.IsTable(),
			"Error serializing field '%s': Can not serialize to object of this type; expected 'table' got '%s'",
			mName.c_str(), table.TypeName());

		// Acquire value reference from holder pointer & offset
		ValueType& ref = getRef(holder);

		// Serialize the field
		serializeImpl(state, holder, ref, table);
	}

	void deserialize(lua::State* state, void* holderPtr, LuaObject table){
		WT_ASSERT(table.IsTable(),
			"Error deserializing field '%s': Can not deserialize from object of this type; expected 'table' got '%s'",
			mName.c_str(), table.TypeName());

		// Acquire the lua object for this field
		LuaObject luaVal = table.Get(mName.c_str());

		// Valid object ?
		WT_ASSERT(!luaVal.IsNil(),
			"Error deserializing field '%s': Missing table entry",
			mName.c_str());

		// Acquire value reference from holder pointer & offset
		ValueType& ref = getRef(holderPtr);

		// Deserialize the field
		deserializeImpl(state, holderPtr, ref, luaVal);
	}

private:
	/**
	 * General deserialization (e.g. primitives, strings etc..)
	 */
	template<class T>
	void deserializeImpl(lua::State* state, void* holder, T& ref, LuaObject luaVal){
		if(!lua::luaConv(luaVal, ref)){
			WT_ASSERT(!luaVal.IsNil(),
				"Error deserializing primitive field '%s': Value conversion failed",
				mName.c_str());
		}
	}

	/**
	 * LuaObject
	 */
	void deserializeImpl(lua::State* state, void* holder, LuaObject& v, LuaObject luaVal){
		v = luaVal;
	}

	/**
	 * Serializator
	 */
	void deserializeImpl(lua::State* state, void* holder, ISerializator& v, LuaObject luaVal){
		v.deserialize(state, luaVal);
	}

	/**
	 * STL vector
	 */
	template<class T>
	void deserializeImpl(lua::State* state, void* holder, std::vector<T>& vec, LuaObject luaVal){
		WT_ASSERT(luaVal.IsTable(),
			"Error deserializing vector field '%s': Given value is '%s', expected 'table'",
			mName.c_str(), luaVal.TypeName());

		for(uint32_t i=0; i<luaVal.GetCount(); i++){
			LuaObject elem = luaVal.Get(i + 1);

			WT_ASSERT(!elem.IsNil(),
				"Error deserializing vector field '%s': Vector element missing (index %d)",
				mName.c_str(), i+1);

			T val;
			WT_ASSERT(lua::luaConv(elem, val),
				"Error deserializing vector field '%s': Element conversion failed (index %d)",
				mName.c_str(), i+1);

			vec.push_back(val);
		}
	}

	/**
	 * LuaObject
	 */
	void serializeImpl(lua::State* state, void* holder, const LuaObject& value, LuaObject table){
		table.Set(mName.c_str(), value);
	}


	/**
	 * STL vector
	 */
	template<class T>
	void serializeImpl(lua::State* state, void* holder, const std::vector<T>& v, LuaObject table){
		uint32_t idx = 1;
		LuaObject vecTable;
		state->assignTable(vecTable);

		for(std::vector<T>::const_iterator iter=v.cbegin(); iter!=v.cend(); iter++){
			vecTable.Set(idx++, *iter);
		}

		table.Set(mName.c_str(), vecTable);
	}

	/**
	 * Serializator
	 */
	void serializeImpl(lua::State* state, void* holder, const ISerializator& value, LuaObject table){
		LuaObject serTable;
		state->assignTable(serTable);

		const_cast<ISerializator&>(value).serialize(state, serTable);
		
		table.Set(mName.c_str(), serTable);
	}

	/**
	 * STL String
	 */
	void serializeImpl(lua::State* state, void* holder, const String& value, LuaObject table){
		table.Set(mName.c_str(), value.c_str());
	}

	/**
	 * int32_t
	 */
	void serializeImpl(lua::State* state, void* holder, int32_t value, LuaObject table){
		table.Set(mName.c_str(), value);
	}

	/**
	 * float
	 */
	void serializeImpl(lua::State* state, void* holder, float value, LuaObject table){
		table.Set(mName.c_str(), value);
	}

	/**
	 * Acquires the variable reference for this specific holder using the
	 * previously set variable offset
	 */
	ValueType& getRef(void* holder){
		return (reinterpret_cast<ValueType&>( *( static_cast<uint8_t*>(holder) + mOffset ) ));
	}

private:
	String mName;
	uint32_t mOffset;

}; // </Field>

} // </wt>

#endif // </WT_FIELD_H>
