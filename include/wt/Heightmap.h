#ifndef WT_HEIGHTMAP_H
#define WT_HEIGHTMAP_H

#include "wt/AResourceManager.h"
#include "wt/Buffer.h"
#include "wt/Singleton.h"

namespace wt
{

class Heightmap : public AResource<Heightmap>{
public:
	Heightmap(AResourceManager<Heightmap>* manager=NULL, ResourceHandle handle=0, const String& name="");

	~Heightmap();

	int16_t* getSamples();

	const int16_t* getSamples() const;

	float getRowScale() const;

	float getHeightScale() const;

	float getColumnScale() const;

	void setRowScale(float scale);

	void setHeightScale(float scale);

	void setColumnScale(float scale);

	uint32_t getNumRows() const;

	uint32_t getNumColumns() const;

	void create(uint32_t numRows, uint32_t numColumns, int16_t initialValue=0);

private:
	typedef Buffer<int16_t> SampleBuffer;
	SampleBuffer mSamples;
	uint32_t mNumRows;
	uint32_t mNumColumns;
	float mHeightScale;
	float mColumnScale;
	float mRowScale;
}; // </Heightmap>

class HeightmapLoader : public AResourceLoader<Heightmap>, public Singleton<HeightmapLoader>{
public:
	void load(AIOStream* stream, Heightmap* dst);

	void save(AIOStream* stream, Heightmap* src);
}; // </HeightmapLoader>

} // </wt>

#endif // </WT_HEIGHTMAP_H>
