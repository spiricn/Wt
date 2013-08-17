#ifndef WT_MATERIAL_H
#define WT_MATERIAL_H

#include "wt/Common.h"
#include "wt/Color.h"

namespace wt{


class Material{
private:
	CullMode mCullMode;
	Color mAmbientColor;
	Color mDiffuseColor;
	Color mSpecularColor;
	float mShininess;

	bool mAlphaTested;
	bool mWireframe;
	bool mBlend;

public:
	Material() : mCullMode(eCULL_CLOCKWISE), mAmbientColor(Color::White()), mSpecularColor(Color::Black()), mShininess(0.0f), 
		mDiffuseColor(Color::White()), mAlphaTested(true), mWireframe(false), mBlend(false){
	}

	void setBlend(bool state){
		mBlend = state;
	}

	bool getBlend() const{
		return mBlend;
	}

	float getShininess() const{
		return mShininess;
	}

	void setShininess(float shininess){
		mShininess = shininess;
	}

	const Color& getSpecular() const{
		return mSpecularColor;
	}

	void setSpecular(const Color& color){
		mSpecularColor = color;
	}

	void setWireframe(bool state){
		mWireframe = state;
	}

	bool isWireframe() const{
		return mWireframe;
	}

	void setAlphaTest(bool state){
		mAlphaTested = state;
	}

	void setCullMode(CullMode cullMode){
		mCullMode = cullMode;
	}

	void setAmbient(const Color& color){
		mAmbientColor = color;
	}

	void setDiffuse(const Color& color){
		mDiffuseColor = color;
	}

	const Color& getAmbient() const{
		return mAmbientColor;
	}

	const Color& getDiffuse() const{
		return mDiffuseColor;
	}

	bool isAlphaTested() const{
		return mAlphaTested;
	}

}; // </Material>


}; // </wt>

#endif // </WT_MATERIAL_H>