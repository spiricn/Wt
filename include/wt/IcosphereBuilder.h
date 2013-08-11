#ifndef WT_ICOSPHEREBUILDER_H
#define WT_ICOSPHEREBUILDER_H

#include "wt/stdafx.h"

namespace wt
{

class IcosphereBuilder{
public:
#pragma pack(push)
#pragma pack(1)
	struct Vertex{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texture;
		glm::vec3 tangent;

		Vertex(float x=0, float y=0, float z=0) : position(x, y, z){
		}
	}; // </Vertex>
#pragma pack(pop)

	IcosphereBuilder(Vertex** vertices, uint32_t** indices, uint32_t* numVertices, uint32_t* numIndices, uint32_t recursionLevel=1);

private:

	struct Face{
		uint32_t v1;
		uint32_t v2;
		uint32_t v3;

		Face(uint32_t v1=0, uint32_t v2=0, uint32_t v3=0) : v1(v1), v2(v2), v3(v3){
		}
	};

	typedef std::map<uint64_t, int32_t> PointCacheMap;
	typedef std::vector<Vertex> VertexList;
	typedef std::vector<uint32_t> IndexList;

	VertexList mVertices;
	PointCacheMap middlePointIndexCache;
	uint32_t mIndex;

	// add vertex to mesh, fix position to be on unit sphere, return index
    uint32_t addVertex(const Vertex& p);

	// return index of point in the middle of p1 and p2
    uint32_t getMiddlePoint(uint32_t p1, uint32_t p2);

}; // </IcosphereBuilder>

} // </wt>

#endif // </WT_ICOSPHEREBUILDER_H>