#ifndef WT_GEOMETRY_H
#define WT_GEOMETRY_H

#include "wt/stdafx.h"

#include "wt/GLBatch.h"
#include "wt/AResourceManager.h"
#include "wt/Singleton.h"
#include "wt/Buffer.h"
#include "wt/Vertex.h"
#include "wt/BoundingBox.h"

namespace wt
{

class Geometry{
public:
	friend class Model;

public:
	typedef Buffer<Vertex> VertexBuffer;

	typedef Buffer<uint32_t> IndexBuffer;

public:
	Geometry(const String& name, gl::Batch* batch, const gl::SubBatch& subBatch, const VertexBuffer& vertices,
		const IndexBuffer& indices);

	gl::Batch* getBatch();

	VertexBuffer& getVertices();

	IndexBuffer& getIndices();

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

#endif // </WT_GEOMETRY_H>
