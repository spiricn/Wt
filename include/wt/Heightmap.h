#ifndef WT_HEIGHTMAP_H
#define WT_HEIGHTMAP_H

#include "wt/AResourceManager.h"
#include "wt/Buffer.h"
namespace wt
{

class Heightmap : public AResource<Heightmap>{
public:
	Heightmap(AResourceManager<Heightmap>* manager=NULL, ResourceHandle handle=0, const String& name="");

	~Heightmap();

	void serialize(lua::State* luaState, LuaPlus::LuaObject& dst);

	void deserialize(lua::State* luaState, const LuaPlus::LuaObject& table);

	int16_t* getSamples();

	const int16_t* getSamples() const;

private:
	typedef Buffer<int16_t> SampleBuffer;
	SampleBuffer mSamples;
	uint32_t mNumRows;
	uint32_t mNumColumns;
	float mHeightScale;
	float mColumnScale;
	float mRowScale;
}; // </Heightmap>

class HeightmapManager : public AResourceManager<Heightmap>{
public:
	HeightmapManager(AResourceSystem* assets) : AResourceManager(assets){
	}
}; // </HeightmapManager>


} // </wt>

#endif // </WT_HEIGHTMAP_H>
