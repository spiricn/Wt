#include "wt/stdafx.h"

#include "wt/TerrainNode.h"
#include "wt/Exception.h"

namespace wt
{

void TerrainNode::calcBounds(){
	mBounds.setEmpty();
	for(uint32_t i=0; i<mChunks.size(); i++){
		mBounds.include( mChunks[i]->mBounds );
	}
}

TerrainNode::TerrainNode(TerrainNode* parent, uint32_t idx, gl::Batch* geoBatch) : mParent(parent),
	mChildren(NULL), mIndex(idx), mSize(0), mBatch(geoBatch){

	if(parent != NULL){
		mSize = parent->mSize/2;

		for(uint32_t i=0; i<mSize; i++){
			for(uint32_t j=0; j<mSize; j++){

				// top left
				if(idx==0){
					mChunks.push_back(  parent->mChunks[ (i+0)*parent->mSize + (j+0) ] );
				}
				// top right
				else if(idx==1){
					mChunks.push_back(  parent->mChunks[ (i+0)*parent->mSize + (j+mSize) ] );
				}
				// botom right
				else if(idx==2){
					mChunks.push_back(  parent->mChunks[ (i+mSize)*parent->mSize + (j+mSize) ] );
				}
				// bottom left
				else{
					mChunks.push_back(  parent->mChunks[ (i+mSize)*parent->mSize + (j+0) ] );
				}
			}
		}


		calcBounds();
	}
}

TerrainNode::~TerrainNode(){
	// TODO DESTROY
}

void TerrainNode::destroy(){
	if(!mChildren != NULL){
		for(uint32_t i=0; i<4; i++){
			mChildren[i].destroy();
		}

		delete[] mChildren;
		mChildren=NULL;
	}
}

void TerrainNode::split(){
	if(mChunks.size()>1){
		WT_ASSERT( mChunks.size() % 4 == 0, "Invalid chunk size %d", mChunks.size());
		//LOG("SPLIT");
		mChildren = new TerrainNode[4];

		for(uint32_t i=0; i<4; i++){
 			mChildren[i] = TerrainNode(this, i);
			mChildren[i].mBatch = mBatch;
		}

		mChunks.clear();

		for(uint32_t i=0; i<4; i++){
			mChildren[i].split();
		}
	}
}

const physx::PxBounds3& TerrainNode::getBounds() const{
	return mBounds;
}

bool TerrainNode::isLeaf() const{
	return mChildren == NULL;
}

const TerrainNode* TerrainNode::getChildren() const{
	return mChildren;
}

void TerrainNode::render() const{
	mBatch->render(0,
		mChunks[0]->mIndices.getData(),
		mChunks[0]->mIndices.getCapacity()
		);
}

} // </wt>