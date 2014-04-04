#ifndef WT_LUAPHYSICS_H
#define WT_LUAPHYSICS_H

namespace wt
{

namespace lua
{

void Physics_expose(LuaPlus::LuaObject state);

void* Physics_getSceneActor(void* physicsActorPtr);

const char* Physics_getActorName(void* physicsActorPtr);

} // </lua>

} // </wt>

#endif // </WT_LUAPHYSICS_H>
