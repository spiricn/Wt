#include "wt/stdafx.h"
#include "wt/lua/LuaEngine.h"
#include "wt/AEngineFramework.h"

#define TD_TRACE_TAG "LuaEngine"

#define ENGINE_OBJ(name, ptr) \
	WT_ASSERT(ptr != NULL, "Invalid engine object pointer"); \
	AEngineFramework* name = static_cast<AEngineFramework*>(ptr);

namespace wt
{

namespace lua
{

void Engine_expose(LuaObject obj){
	MODULE_EXPOSE(obj, Engine_getInput);
}

void* Engine_getInput(void* enginePtr){
	ENGINE_OBJ(engine, enginePtr);

	return static_cast<void*>( engine->getInput() );
}

} // </lua>

} // </wt>

