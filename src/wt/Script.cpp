#include "wt/stdafx.h"

#include "wt/lua/Script.h"

namespace wt{

namespace lua{

Script::Script(State* parent) : mParent(parent){
}

Script::~Script(){
	// TODO delete script enviroment?
}


LuaObject& Script::getState(){
	return mState;
}

const LuaObject& Script::getState() const{
	return mState;
}

State* Script::getParent() const{
	return mParent;
}

}; // </lua>

}; // </wt>
