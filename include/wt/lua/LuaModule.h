#ifndef WT_LUAMODULE_H
#define WT_LUAMODULE_H

#define MODULE_EXPOSE(state, fnc) do{ state.RegisterDirect(# fnc, fnc); } while(0)

namespace wt
{

namespace lua
{

} // </lua>

} // </wt>

#endif // </WT_LUAMODULE_H>
