#include "wt/stdafx.h"

#include "wt/SkeletonBone.h"

#define TD_TRACE_TAG "SkeletonBone"

namespace wt{

glm::mat4& SkeletonBone::getOffset(){
	return mOffset;
}


void SkeletonBone::setName(const String& name){
	mName = name;
}

SkeletonBone::SkeletonBone(SkeletonBone* parent, const String& name,
	Uint32 index) : ANode(parent, name), mIndex(index){
}

void SkeletonBone::setTransform(const glm::mat4& transform){
	mTransform = transform;
}

const glm::mat4& SkeletonBone::getTransform() const{
	return mTransform;
}

glm::mat4& SkeletonBone::getTransform(){
	return mTransform;
}

Uint32 SkeletonBone::getIndex() const{
	return mIndex;
}

void SkeletonBone::duplicate(SkeletonBone* dst){
	dst->mOffset = mOffset;
	dst->mTransform= mTransform;
	dst->mIndex = mIndex;

	for(Iterator i=getBeg(); i!=getEnd(); i++){
		SkeletonBone* child = dst->addChild( (*i)->mName, (*i)->mIndex );
		(*i)->duplicate(child);
	}
}

void SkeletonBone::getGlobalTransform(glm::mat4& dst){
	// Start off with this bone's local transformation
	dst = mTransform;

	// If it has no parent, we already have it's global transform
	if(mParent==NULL){
		return;
	}

	// If not, recursivley concatenate with all the bone's predecessors
	SkeletonBone* node = mParent;
	do{
		dst = node->mTransform * dst;
		node = node->mParent;
	}while(node != NULL);
}

SkeletonBone* SkeletonBone::addChild(const String& name, Uint32 index){
	return ANode::addChild(name, new SkeletonBone(this, name, index));
}

void SkeletonBone::calculateBoneMatrx(Buffer<glm::mat4x4>& matrices){
	/* recursivley calculate the global world transform of this joint */
	getGlobalTransform( matrices[mIndex]) ;
	

	/* transform it from bind position */
	matrices[mIndex] *= mOffset;

	/* do the same thing for all the children */
	for(Iterator i=getBeg(); i!=getEnd(); i++){
		(*i)->calculateBoneMatrx( matrices);
	}
}


}; // <wt>
