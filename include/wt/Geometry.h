#ifndef WT_GEOMETRY_H
#define WT_GEOMETRY_H

#include "wt/stdafx.h"

#include "wt/GLBatch.h"
#include "wt/AResourceManager.h"
#include "wt/Singleton.h"
#include "wt/Buffer.h"

namespace wt
{

class Geometry{
public:
	friend class Model;

	//WT_DISALLOW_COPY(Geometry);

	static const int MAX_BONES = 200;

	/* shouldn't do anything since struct size is already 4 byte aligned */
	#pragma pack(push)
	#pragma pack(1)

	struct Vertex{
		/* position */
		float x, y, z;

		/* texture coordinates */
		float s, t;

		/* normal */
		float nx, ny, nz;

		/* bone IDs this vertex is affected by */
		uint8_t bones[4];

		/* bone weights of said bones */
		float weights[4];

		/* tangent vector (relevant if normal map is present) */
		float tx, ty, tz;

		Vertex();

		void addBone(int32_t index, float weight);
	};
	#pragma pack(pop)

	typedef Buffer<Vertex> VertexBuffer;

	typedef Buffer<uint32_t> IndexBuffer;

	gl::Batch* getBatch();

	VertexBuffer& getVertices();

	IndexBuffer& getIndices();

	const VertexBuffer& getVertices() const;

	const IndexBuffer& getIndices() const;

	Geometry(const String& name, gl::Batch* batch, const gl::SubBatch& subBatch, const VertexBuffer& vertices,
		const IndexBuffer& indices);

	const String& getName() const;

	void setName(const String& name);

	void destroy();

	void render();

	const gl::SubBatch& getSubBatch() const;

private:
	String mName;
	gl::Batch* mBatch;
	gl::SubBatch mSubBatch;
	VertexBuffer mVertices;
	IndexBuffer mIndices;

}; // </Geometry>

} // </wt>

#endif
