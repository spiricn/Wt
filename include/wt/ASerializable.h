#ifndef WT_ASERIALIZABLE_H
#define WT_ASERIALIZABLE_H

namespace wt{

namespace lua{
	class State;
};

namespace lua{

class ASerializable{
public:
	virtual void serialize(lua::State* luaState, LuaPlus::LuaObject& dst)=0;

	virtual void deserialize(lua::State* luaState, const LuaPlus::LuaObject& src)=0;
}; // </Serializable>

} // </Lua>

} // <wt>

#endif