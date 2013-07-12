#ifndef WT_SKELETONBONE_H
#define WT_SKELETONBONE_H


#include "wt/stdafx.h"


#include "wt/ANode.h"
#include "wt/Buffer.h"


namespace wt{

class SkeletonBone : public ANode<SkeletonBone>{
public:

private:
	glm::mat4 mOffset;
	glm::mat4 mTransform;
	Uint32 mIndex;

public:
	glm::mat4& getOffset();

	const glm::mat4& getTransform() const;

	glm::mat4& getTransform();

	void setName(const String& name);

	SkeletonBone(SkeletonBone* parent, const String& name,
		Uint32 index);

	void setTransform(const glm::mat4& transform);

	Uint32 getIndex() const;

	void duplicate(SkeletonBone* dst);

	void getGlobalTransform(glm::mat4& dst);

	SkeletonBone* addChild(const String& name, Uint32 index);

	void calculateBoneMatrx(Buffer<glm::mat4x4>& matrices);

}; // </SkeletonBone>


}; // <wt>

#endif
