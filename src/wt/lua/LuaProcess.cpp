#include "wt/stdafx.h"

#include "wt/lua/LuaProcess.h"
#include "wt/ScriptProcess.h"

#define TD_TRACE_TAG "LuaProcess"

#define PROC_OBJ(name, obj) \
	AProcess* name = static_cast<AProcess*>(obj); \
	WT_ASSERT(name != NULL, "Invalid AProcess object pointer");


#define MNGR_OBJ(name, obj) \
	ProcessManager* name = static_cast<ProcessManager*>(obj); \
	WT_ASSERT(name != NULL, "Invalid ProcessManager object pointer");

namespace wt
{

namespace lua
{

void Process_expose(LuaObject obj){
	MODULE_EXPOSE(obj, Process_create);
	MODULE_EXPOSE(obj, Process_kill);
	MODULE_EXPOSE(obj, Process_setNext);
	MODULE_EXPOSE(obj, Process_attach);
	MODULE_EXPOSE(obj, Process_suspend);
	MODULE_EXPOSE(obj, Process_spawn);
}

void* Process_spawn(void* managerPtr, LuaObject startFnc, LuaObject updateFnc, LuaObject endFnc, LuaObject luaName){
	MNGR_OBJ(procManager, managerPtr);
	PROC_OBJ(proc, Process_create(managerPtr, startFnc, updateFnc, endFnc, luaName) );

	procManager->attach(proc);

	return static_cast<void*>(proc);
}

void Process_suspend(void* procPtr, bool state){
	PROC_OBJ(proc, procPtr);

	proc->suspend(state);
}

void* Process_create(void* managerPtr, LuaObject startFnc, LuaObject updateFnc, LuaObject endFnc, LuaObject procName){
	MNGR_OBJ(procManager, managerPtr);

	String name = "";
	lua::luaConv(procName, name);

	ScriptProcess* proc = new ScriptProcess(startFnc, updateFnc, endFnc, name);

	return static_cast<void*>( dynamic_cast<AProcess*>(proc) );
}

void Process_kill(void* procPtr){
	PROC_OBJ(proc, procPtr);

	proc->killProcess();
}

void* Process_setNext(void* proc1Ptr, void* proc2Ptr){
	PROC_OBJ(proc1, proc1Ptr);
	PROC_OBJ(proc2, proc2Ptr);

	proc1->setNext(proc2);

	return proc2;
}

void* Process_attach(void* managerPtr, void* procPtr){
	MNGR_OBJ(procManager, managerPtr);
	PROC_OBJ(proc, procPtr);

	procManager->attach(proc);

	return procPtr;
}

} // </lua>

} // </wt>
