#ifndef WT_LUAMODULE_H
#define WT_LUAMODULE_H

#define MODULE_EXPOSE(state, fnc) do{ state.RegisterDirect(# fnc, fnc); } while(0)

#define ENUM_DEF(state, name, value) do{ state.Set(# name, value); }while(0)

#define LUA_OBJ_DEC(clazz, name, ptr) \
	WT_ASSERT(ptr != NULL, "Invalid " #name " object pointer"); \
	clazz* name = static_cast<clazz*>(ptr);

namespace wt
{

namespace lua
{

} // </lua>

} // </wt>

#endif // </WT_LUAMODULE_H>
