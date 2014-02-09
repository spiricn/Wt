#include "wt/stdafx.h"
#include "wt/Heightmap.h"
#include "wt/Lua.h"

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

	uint32_t numRows, numColumns;
	WT_ASSERT(lua::luaConv(src.Get("num_rows"), numRows), "num_rows field missing");
	WT_ASSERT(lua::luaConv(src.Get("num_columns"), numColumns), "num_columns field missing");

	create(numRows, numColumns);

	lua::luaConv(src.Get("height_scale"), mHeightScale);
	lua::luaConv(src.Get("row_scale"), mRowScale);
	lua::luaConv(src.Get("column_scale"), mColumnScale);
}

float Heightmap::getRowScale() const{
	return mRowScale;
}

float Heightmap::getHeightScale() const{
	return mHeightScale;
}

float Heightmap::getColumnScale() const{
	return mColumnScale;
}

void Heightmap::setRowScale(float scale){
	mRowScale = scale;
}

void Heightmap::setHeightScale(float scale){
	mHeightScale = scale;
}

void Heightmap::setColumnScale(float scale){
	mColumnScale = scale;
}

uint32_t Heightmap::getNumRows() const{
	return mNumRows;
}

uint32_t Heightmap::getNumColumns() const{
	return mNumColumns;
}

void Heightmap::create(uint32_t numRows, uint32_t numColumns, int16_t initialValue){
	mSamples.create(numRows*numColumns);

	for(uint32_t i=0; i<numRows*numColumns; i++){
		mSamples[i] = initialValue;
	}

	mNumRows = numRows;
	mNumColumns = numColumns;
}

void HeightmapLoader::load(AIOStream* stream, Heightmap* dst){
	int64_t numBytes = dst->getNumRows() * dst->getNumColumns() * sizeof(int16_t);
	WT_ASSERT(stream->getSize() == numBytes, "invalid heightmap file size (got %l, expected %l)", stream->getSize(), numBytes);

	WT_ASSERT(stream->read(static_cast<void*>(dst->getSamples()), numBytes) == numBytes, "Error reading heightmap samples");
}

void HeightmapLoader::save(AIOStream* stream, Heightmap* src){
	int64_t numBytes = src->getNumRows() * src->getNumColumns() * sizeof(int16_t);

	stream->write(static_cast<void*>(src->getSamples()), numBytes);
}

} // </wt>

