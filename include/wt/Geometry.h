#ifndef WT_GEOMETRY_H
#define WT_GEOMETRY_H

#include "wt/stdafx.h"

#include "wt/GLBatch.h"
#include "wt/AResourceManager.h"
#include "wt/Singleton.h"
#include "wt/Buffer.h"

namespace wt{

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

		Vertex() : x(0.0f), y(0.0f), z(0.0f), s(0.0f), t(0.0f),
			nx(0.0f), ny(0.0f), nz(0.0f), tx(0.0f), ty(0.0f), tz(0.0f){

			for(uint32_t i=0; i<4; i++){
				weights[i] = 0.0f;
				// bone index of value MAX_BONES indicates that the slot is not taken
				bones[i] = MAX_BONES; 
			}
		}

		void addBone(int32_t index, float weight){
#ifdef WT_CHECKED
			if(index < 0 || index >= MAX_BONES){
				WT_THROW("Bone infdex out of valid range %d (min=0, max=%d)", index, MAX_BONES);
			}
#endif

			// find the first bone slot that's not taken
			for(uint32_t i=0; i<4; i++){
				if(bones[i]==MAX_BONES){
					// assign the index and weight
					bones[i]=index;
					weights[i]=weight;
					return;
				}
			}
#ifdef WT_CHECKED
			WT_THROW("Vertex supports max 4 bones (attempted to add 5th)");
#endif
		}
	};
	#pragma pack(pop)

	typedef Buffer<Vertex> VertexBuffer;
	typedef Buffer<uint32_t> IndexBuffer;
private:
	String mName;
	gl::Batch* mBatch;
	gl::SubBatch mSubBatch;
	VertexBuffer mVertices;
	IndexBuffer mIndices;

public:

	gl::Batch* getBatch(){
		return mBatch;
	}

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
};

}; // </wt>

#endif
