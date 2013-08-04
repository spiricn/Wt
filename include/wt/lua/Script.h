#ifndef WT_LUASCRIPT_H
#define WT_LUASCRIPT_H

#include <LuaPlus.h>

namespace wt{

namespace lua{

using namespace LuaPlus;

class State;

class Script{
friend class State;

public:
	LuaObject& getState();

	const LuaObject& getState() const;

	State* getParent() const;

	~Script();

protected:
	Script(State* parent);

private:
	LuaObject mState;
	State* mParent;

	Script(const Script&);

}; // </Script>

}; // </lua>

}; // </wt>

#endif // </WT_LUASCRIPT_H>