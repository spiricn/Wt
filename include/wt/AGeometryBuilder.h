#ifndef WT_AGEOMETRYBUILDER_H
#define WT_AGEOMETRYBUILDER_H

namespace wt
{

class AGeometryBuilder{
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

	struct Face{
		uint32_t v1;
		uint32_t v2;
		uint32_t v3;

		Face(uint32_t v1=0, uint32_t v2=0, uint32_t v3=0) : v1(v1), v2(v2), v3(v3){
		}
	};

	typedef std::vector<Vertex> VertexList;
	typedef std::vector<uint32_t> IndexList;

#pragma pack(pop)
	
protected:
	VertexList mVertices;
	IndexList mIndices;

}; // </AGeometryBuilder>

} // </wt>

#endif // </WT_AGEOMETRYBUILDER_H>