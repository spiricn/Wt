#include "wt/stdafx.h"

#include "wt/MatrixStack.h"

namespace wt{

namespace math{

MatrixStack::MatrixStack() : mStackPtr(0), mDepth(256){
	mData = new float[mDepth*16];
	memset(mData, 0x00, 16*mDepth*sizeof(float));
	pushIdentity();
}

MatrixStack::~MatrixStack(){
	delete[] mData;
	mData=NULL;
}

void MatrixStack::push(const glm::mat4x4& mat){
	push(glm::value_ptr(mat));
}

void MatrixStack::push(const math::Transform& transform){
	glm::mat4x4 m;
	transform.getMatrix(m);
	push(m);
}

void MatrixStack::push(const float* data){
	if(mStackPtr > mDepth-1){
		WT_EXCEPT("MatrixStack", "Maximum stack depth exceeded ");
	}

	memcpy(mData + (mStackPtr++)*16*sizeof(float),
		data, 16*sizeof(float));
}

void MatrixStack::pop(glm::mat4x4& dst){
	pop(glm::value_ptr(dst));
}


void MatrixStack::top(glm::mat4x4& dst) const{
	top(glm::value_ptr(dst));
}

void MatrixStack::top(float* data) const{
	memcpy(data,
		mData + (mStackPtr-1)*16*sizeof(float), 16*sizeof(float));
}

void MatrixStack::pop(float* data){
	if(isEmpty()){
		WT_EXCEPT("MatrixStack", "Pop operation over empty stack");
	}

	mStackPtr--;
	top(data);
}

void MatrixStack::pop(){
	if(isEmpty()){
		WT_EXCEPT("MatrixStack", "Pop operation over empty stack");
	}

	mStackPtr--;
}

void MatrixStack::pushIdentity(){
	static const float MAT_IDENTITY[16] = {	1,0,0,0,
											0,1,0,0,
											0,0,1,0,
											0,0,0,1
	};
	push(MAT_IDENTITY);
}

bool MatrixStack::isEmpty() const{
	return mStackPtr==0;
}

Uint32 MatrixStack::getSize() const{
	return mStackPtr;
}

void MatrixStack::postConcat(const glm::mat4x4& b){
	glm::mat4x4 a;
	top(a);

	a = a*b;
	pop();
	push(a);
}

void MatrixStack::preConcat(const glm::mat4x4& a){
	glm::mat4x4 b;
	top(b);

	b = a*b;
	pop();
	push(b);
}

void MatrixStack::pushAndConcat(const glm::mat4x4& mat){
	glm::mat4x4 top;
	this->top(top);

	top = top*mat;
	push(top);
}

void MatrixStack::pushAndConcat(const math::Transform& transform){
	glm::mat4x4 t;
	transform.getMatrix(t);

	pushAndConcat(t);
}

void MatrixStack::pushAndConcat(math::Camera& camera){
	glm::mat4x4 t;
	camera.getMatrix(t, false);

	pushAndConcat(t);
}

}; // </math>

}; // </wt>
