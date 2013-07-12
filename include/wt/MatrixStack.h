#ifndef WT_MATRIXSTACK_H
#define WT_MATRIXSTACK_H


#include "wt/stdafx.h"


#include "wt/Exception.h"
#include "wt/Camera.h"
#include "wt/Transform.h"
#include "wt/Defines.h"

namespace wt{

namespace math{

class MatrixStack{
private:
	WT_DISALLOW_COPY(MatrixStack)

	float* mData;
	uint32_t mStackPtr;
	uint32_t mDepth;

public:
	MatrixStack();

	~MatrixStack();

	bool isEmpty() const;

	uint32_t getSize() const;

	glm::mat4x4& top();

	void top(glm::mat4x4& dst) const;

	void top(float* data) const;

	void push(const glm::mat4x4& mat);

	void push(const math::Transform& transform);

	void push(const float* data);

	void pop(glm::mat4x4& dst);

	void pop(float* data);

	void pop();

	void pushIdentity();

	void postConcat(const glm::mat4x4& b);

	void preConcat(const glm::mat4x4& a);

	void pushAndConcat(const glm::mat4x4& mat);

	void pushAndConcat(const math::Transform& transform);

	void pushAndConcat(math::Camera& camera);

}; // </MatrixStack>


}; // </math>

}; // </wt>

#endif;
