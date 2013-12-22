#include "wt/stdafx.h"

#include "wt/Geometry.h"

#define TD_TRACE_TAG "Geometry"

namespace wt{
	
Geometry::Vertex::Vertex() : x(0.0f), y(0.0f), z(0.0f), s(0.0f), t(0.0f),
	nx(0.0f), ny(0.0f), nz(0.0f), tx(0.0f), ty(0.0f), tz(0.0f){

	for(uint32_t i=0; i<4; i++){
		weights[i] = 0.0f;
		// bone index of value MAX_BONES indicates that the slot is not taken
		bones[i] = MAX_BONES; 
	}
}

void Geometry::Vertex::addBone(int32_t index, float weight){
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

gl::Batch* Geometry::getBatch(){
	return mBatch;
}


Geometry::VertexBuffer& Geometry::getVertices(){
	return mVertices;
}

Geometry::IndexBuffer& Geometry::getIndices(){
	return mIndices;
}

const Geometry::VertexBuffer& Geometry::getVertices() const{
	return mVertices;
}

const Geometry::IndexBuffer& Geometry::getIndices() const{
	return mIndices;
}

Geometry::Geometry(const String& name, gl::Batch* batch, const gl::SubBatch& subBatch, const VertexBuffer& vertices,
	const IndexBuffer& indices) : mName(name), mBatch(batch), mSubBatch(subBatch){

	vertices.copy(mVertices);
	indices.copy(mIndices);
}

const String& Geometry::getName() const{
	return mName;
}

void Geometry::setName(const String& name){
	mName = name;
}

void Geometry::destroy(){
	mName="";
}

void Geometry::render(){
	mBatch->render(mSubBatch);
}

const gl::SubBatch& Geometry::getSubBatch() const{
	return mSubBatch;
}

}; // </wt>
