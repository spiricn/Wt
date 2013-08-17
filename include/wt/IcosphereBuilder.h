#ifndef WT_ICOSPHEREBUILDER_H
#define WT_ICOSPHEREBUILDER_H

#include "wt/stdafx.h"

#include "wt/AGeometryBuilder.h"

namespace wt
{

class IcosphereBuilder : public AGeometryBuilder{
public:

	IcosphereBuilder(Vertex** vertices, uint32_t** indices, uint32_t* numVertices, uint32_t* numIndices, uint32_t recursionLevel=1);

private:
	typedef std::map<uint64_t, int32_t> PointCacheMap;
	
	PointCacheMap mMiddlePointIndexCache;
	uint32_t mIndex;

	// add vertex to mesh, fix position to be on unit sphere, return index
    uint32_t addVertex(const Vertex& p);

	// return index of point in the middle of p1 and p2
    uint32_t getMiddlePoint(uint32_t p1, uint32_t p2);

}; // </IcosphereBuilder>

} // </wt>

#endif // </WT_ICOSPHEREBUILDER_H>