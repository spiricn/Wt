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
	int64_t rc = 0;

	// Dimensions
	uint32_t numRows = 0;
	rc = stream->read(&numRows, sizeof(uint32_t));
	WT_ASSERT(rc == sizeof(uint32_t), "Error reading heightmap file");

	uint32_t numColumns = 0;
	rc = stream->read(&numColumns, sizeof(uint32_t));
	WT_ASSERT(rc == sizeof(uint32_t), "Error reading heightmap file");

	WT_ASSERT(numRows > 0 && numColumns > 0, "Invalid heightmap dimensions (%u x %u)", numRows, numColumns);

	// Scaling
	float heightScale = 0.0f;
	rc = stream->read(&heightScale, sizeof(float));
	WT_ASSERT(rc == sizeof(float), "Error reading heightmap file");

	float columnScale = 0.0f;
	rc = stream->read(&columnScale , sizeof(float));
	WT_ASSERT(rc == sizeof(float), "Error reading heightmap file");

	float rowScale = 0.0f;
	rc = stream->read(&rowScale, sizeof(float));
	WT_ASSERT(rc == sizeof(float), "Error reading heightmap file");

	// Samples
	int64_t numBytes = numRows * numColumns * sizeof(int16_t);
	dst->create(numRows, numColumns);
	rc = stream->read(dst->getSamples(), numBytes);
	WT_ASSERT(numBytes == rc, "Error reading heightmap file");
}

void HeightmapLoader::save(AIOStream* stream, Heightmap* src){
	// Dimensions
	uint32_t numRows = src->getNumRows();
	stream->write(&numRows, sizeof(uint32_t));

	uint32_t numColumns = src->getNumColumns();
	stream->write(&numColumns, sizeof(uint32_t));

	// Scaling
	float heightScale = src->getHeightScale();
	stream->write(&heightScale, sizeof(float));

	float columnScale = src->getColumnScale();
	stream->write(&columnScale , sizeof(float));

	float rowScale = src->getRowScale();
	stream->write(&rowScale, sizeof(float));

	// Samples
	stream->write(static_cast<void*>(src->getSamples()), src->getNumRows() * src->getNumColumns() * sizeof(int16_t));
}

} // </wt>

