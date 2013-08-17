#include "wt/stdafx.h"

#include "wt/ALight.h"
#include "wt/Color.h"
#include "wt/Lua.h"

#define TD_TRACE_TAG "ALight"

namespace wt
{

using namespace LuaPlus;

ALight::~ALight(){
}

ALight::Desc::~Desc(){
}

void ALight::Desc::serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const{
	LuaObject luaColor = luaState->newTable();

	lua::luaConv(color, luaColor);
	dst.Set("color", luaColor);

	dst.Set("ambientIntensity", ambientIntensity);

	dst.Set("diffuseIntensity", diffuseIntensity);
}

void ALight::Desc::deserialize(lua::State* luaState, const LuaPlus::LuaObject& src){
	WT_ASSERT(src.IsTable()
		&& src.Get("ambientIntensity").IsNumber()
		&& src.Get("diffuseIntensity").IsNumber()
		&& src.Get("color").IsTable(),
		"Invalid light lua table");

	if(!lua::luaConv(src.Get("color"), color)){
		// TODO handle
	}

	if(!lua::luaConv(src.Get("ambientIntensity"), ambientIntensity)){
		// TODO handle
	}

	if(!lua::luaConv(src.Get("diffuseIntensity"), diffuseIntensity)){
		// TODO handle
	}
}

ALight::ALight(Scene* parent, uint32_t id, Type type) : mParent(parent), mId(id), mType(type){
}

Scene* ALight::getParent() const{
	return mParent;
}

uint32_t ALight::getId() const{
	return mId;
}

ALight::Type ALight::getType() const{
	return mType;
}

ALight::Desc::Desc() : color(Color::White()), enabled(true), ambientIntensity(1.0f), diffuseIntensity(1.0f){
}

} // </wt>
