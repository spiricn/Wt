#ifndef WT_SCRIPTPROCESS_H
#define WT_SCRIPTPROCESS_H

#include "wt/AProcess.h"
#include "wt/lua/State.h"
#include "wt/OwnPtr.h"

namespace wt{

class ScriptProcess : public AProcess{
public:
	ScriptProcess(LuaObject startFnc, LuaObject updateFnc, LuaObject endFnc, const String& name="");

	~ScriptProcess();

	void onProcUpdate(float dt);

	void onProcStart();

	void onProcEnd();

private:
	typedef lua::LuaFunction<void> CallbackFnc;
	typedef OwnPtr<CallbackFnc> CallbackPtr;

	CallbackPtr mStartFnc;
	CallbackPtr mEndFnc;
	CallbackPtr mUpdateFnc;

}; // </ScriptProcess>

}; // </wt>

#endif // </WT_SCRIPTPROCESS_H>