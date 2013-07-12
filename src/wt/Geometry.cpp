#include "wt/stdafx.h"

#include "wt/Geometry.h"

namespace wt{

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

Geometry::Geometry(const String& name, Gl::Batch* batch, const Gl::SubBatch& subBatch, const VertexBuffer& vertices,
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

const Gl::SubBatch& Geometry::getSubBatch() const{
	return mSubBatch;
}

}; // </wt>
