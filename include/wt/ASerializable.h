#ifndef WT_ASERIALIZABLE_H
#define WT_ASERIALIZABLE_H

namespace wt{

namespace Lua{

class ASerializable{
public:
	virtual void serialize(LuaPlus::LuaObject& dst)=0;

	virtual void deserialize(const LuaPlus::LuaObject& src)=0;
}; // </Serializable>

} // </Lua>

} // <wt>

#endif