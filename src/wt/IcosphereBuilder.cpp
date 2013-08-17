#include "wt/stdafx.h"

#include "wt/IcosphereBuilder.h"
#include "wt/Buffer.h"
#include "wt/Math.h"

namespace wt
{

IcosphereBuilder::IcosphereBuilder(Vertex** vertices, uint32_t** indices, uint32_t* numVertices, uint32_t* numIndices, uint32_t recursionLevel){
	mIndex = 0;

	// create 12 vertices of a icosahedron
	float t = (1.0 + glm::sqrt(5.0)) / 2.0;

	addVertex( Vertex(-1,  t,  0) );
	addVertex( Vertex( 1,  t,  0) );
	addVertex( Vertex(-1, -t,  0) );
	addVertex( Vertex( 1, -t,  0) );

	addVertex( Vertex( 0, -1,  t) );
	addVertex( Vertex( 0,  1,  t) );
	addVertex( Vertex( 0, -1, -t) );
	addVertex( Vertex( 0,  1, -t) );

	addVertex( Vertex( t,  0, -1) );
	addVertex( Vertex( t,  0,  1) );
	addVertex( Vertex(-t,  0, -1) );
	addVertex( Vertex(-t,  0,  1) );

	
	// create 20 triangles of the icosahedron
	Buffer<Face>* faces = new Buffer<Face>(20);
	
	// 5 faces around point 0
	faces->put( Face(0, 11, 5) );
	faces->put( Face(0, 5, 1) );
	faces->put( Face(0, 1, 7) );
	faces->put( Face(0, 7, 10) );
	faces->put( Face(0, 10, 11) );

	// 5 adjacent faces 
	faces->put( Face(1, 5, 9) );
	faces->put( Face(5, 11, 4) );
	faces->put( Face(11, 10, 2) );
	faces->put( Face(10, 7, 6) );
	faces->put( Face(7, 1, 8) );

	// 5 faces around point 3
	faces->put( Face(3, 9, 4) );
	faces->put( Face(3, 4, 2) );
	faces->put( Face(3, 2, 6) );
	faces->put( Face(3, 6, 8) );
	faces->put( Face(3, 8, 9) );

	// 5 adjacent faces 
	faces->put( Face(4, 9, 5) );
	faces->put( Face(2, 4, 11) );
	faces->put( Face(6, 2, 10) );
	faces->put( Face(8, 6, 7) );
	faces->put( Face(9, 8, 1) );

	for(int i=0; i<recursionLevel; i++){
		Buffer<Face>* faces2 = new Buffer<Face>(faces->getCapacity()*4);

		for(uint32_t j=0; j<faces->getCapacity(); j++){
			// replace each triangle of the previous sphere by 4 refined triangles
			const Face& tri = faces->get(j);

			int a = getMiddlePoint(tri.v1, tri.v2);
			int b = getMiddlePoint(tri.v2, tri.v3);
			int c = getMiddlePoint(tri.v3, tri.v1);

			faces2->put( Face(tri.v1, a, c) );
			faces2->put( Face(tri.v2, b, a) );
			faces2->put( Face(tri.v3, c, b) );
			faces2->put( Face(a, b, c) );
		}

		Buffer<Face>* tmp = faces;
		faces = faces2;

		delete tmp;
	}

	*numIndices = faces->getCapacity()*3;
	*numVertices = mVertices.size();

	*vertices = new Vertex[*numVertices];
	*indices= new uint32_t[*numIndices];
		
	for(uint32_t i=0; i<mVertices.size(); i++){
		(*vertices)[i] = mVertices[i];
	}

	for(uint32_t i=0; i<faces->getCapacity(); i++){
		(*indices)[i*3 + 0] = faces->get(i).v1;
		(*indices)[i*3 + 1] = faces->get(i).v2;
		(*indices)[i*3 + 2] = faces->get(i).v3;
	}

	delete faces;
}

// add vertex to mesh, fix position to be on unit sphere, return index
uint32_t IcosphereBuilder::addVertex(const Vertex& v){
	Vertex vertex = v;

	// Normalze the position
	vertex.position = glm::normalize(vertex.position);
	
	// Calculate the normal
	vertex.normal = -vertex.position;

	// Calculate the texture coordinate
#if 1
	vertex.texture.s = glm::asin(vertex.normal.x)/math::PI + 0.5f;
	vertex.texture.t = glm::asin(vertex.normal.y)/math::PI + 0.5f;
#else
	// Faster alternative
	vertex.texture.s = vertex.normal.x/2.0f + 0.5f;
	vertex.texture.t = vertex.normal.y/2.0f + 0.5f;
#endif

	// TODO calculate tangent
	vertex.tangent = glm::vec3(0.0f);

    mVertices.push_back( vertex );

    return mIndex++;
}

// return index of point in the middle of p1 and p2
uint32_t IcosphereBuilder::getMiddlePoint(uint32_t p1, uint32_t p2){
    // first check if we have it already
    bool firstIsSmaller = p1 < p2;
    uint64_t smallerIndex = firstIsSmaller ? p1 : p2;
    uint64_t greaterIndex = firstIsSmaller ? p2 : p1;
    uint64_t key = (smallerIndex << 32) + greaterIndex;

    int ret;

	PointCacheMap::iterator iter = mMiddlePointIndexCache.find(key);
	if(iter != mMiddlePointIndexCache.end()){
		return iter->second;
	}

    // not in cache, calculate it
    const Vertex& point1 = mVertices[p1];
    const Vertex& point2 = mVertices[p2];

    Vertex middle = Vertex(
        (point1.position.x + point2.position.x) / 2.0, 
        (point1.position.y + point2.position.y) / 2.0, 
        (point1.position.z + point2.position.z) / 2.0);

    // add vertex makes sure point is on unit sphere
    uint32_t i = addVertex(middle); 

    // store it, return index
    mMiddlePointIndexCache.insert(
		std::make_pair(key, i)
	);

    return i;
}


} // </wt>