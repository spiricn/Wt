#ifndef WT_SCRIPTPROCESS_H
#define WT_SCRIPTPROCESS_H

#include "wt/AProcess.h"
#include "wt/lua/State.h"

namespace wt{

class ScriptProcess : public AProcess, public lua::Object<ScriptProcess>{
public:
	ScriptProcess(lua::ScriptPtr);

	void onProcUpdate(float dt);

	void onProcStart();

	void onProcEnd();

	void generateMetaTable();

protected:

	void stopProcess();

private:
	lua::ScriptPtr mScript;
	lua::LuaFunction<void> mStartFnc;
	lua::LuaFunction<void> mEndFnc;
	lua::LuaFunction<void> mUpdateFnc;

}; // </ScriptProcess>

}; // </wt>

#endif // </WT_SCRIPTPROCESS_H>