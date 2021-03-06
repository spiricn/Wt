#ifndef WT_LUAPARTICLE_H
#define WT_LUAPARTICLE_H

#include "wt/Lua.h"

namespace wt
{

namespace lua
{

void Particle_expose(LuaObject obj);

void Particle_setVisible(void* ptr, bool state);

void Particle_kill(void* ptr);

} // </lua>

} // </wt>

#endif // </WT_LUAPARTICLE_H>