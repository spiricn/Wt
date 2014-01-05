#include "wt/stdafx.h"
#include "wt/Heightmap.h"

#define TD_TRACE_TAG "Heightmap"

namespace wt
{

Heightmap::Heightmap(AResourceManager<Heightmap>* manager, ResourceHandle handle,
	const String& name) : AResource(manager, handle, name), mNumRows(0), mNumColumns(0), mHeightScale(0.0f), mRowScale(0.0f), mColumnScale(0.0f){
}

Heightmap::~Heightmap(){
}

int16_t* Heightmap::getSamples(){
	return mSamples.getData();
}

const int16_t* Heightmap::getSamples() const{
	return mSamples.getData();
}

void Heightmap::serialize(lua::State* luaState, LuaPlus::LuaObject& dst){
	AResource::serialize(luaState, dst);

	dst.Set("num_rows", mNumRows);
	dst.Set("num_columns", mNumColumns);
	dst.Set("height_scale", mHeightScale);
	dst.Set("row_scale", mRowScale);
	dst.Set("column_scale", mColumnScale);
}

void Heightmap::deserialize(lua::State* luaState, const LuaPlus::LuaObject& src){
	AResource::deserialize(luaState, src);
}
	
} // </wt>

