#include "wt/stdafx.h"

#include "wt/Geometry.h"

#define TD_TRACE_TAG "Geometry"

namespace wt
{
	
gl::Batch* Geometry::getBatch(){
	return mBatch;
}

Geometry::VertexBuffer& Geometry::getVertices(){
	return mVertices;
}

Geometry::IndexBuffer& Geometry::getIndices(){
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
