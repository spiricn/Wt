#include "wt/stdafx.h"
#include "wt/Material.h"

#define TD_TRACE_TAG "Material"

namespace wt
{

Material::Material() : mCullMode(eCULL_NONE /* culling disabled by default */), mAmbientColor(Color::White()), mSpecularColor(Color::Black()), mShininess(0.0f), 
	mDiffuseColor(Color::White()), mAlphaTested(false), mWireframe(false), mBlend(false){
}

void Material::setBlend(bool state){
	mBlend = state;
}

bool Material::getBlend() const{
	return mBlend;
}

float Material::getShininess() const{
	return mShininess;
}

void Material::setShininess(float shininess){
	mShininess = shininess;
}

const Color& Material::getSpecular() const{
	return mSpecularColor;
}

void Material::setSpecular(const Color& color){
	mSpecularColor = color;
}

void Material::setWireframe(bool state){
	mWireframe = state;
}

bool Material::isWireframe() const{
	return mWireframe;
}

void Material::setAlphaTest(bool state){
	mAlphaTested = state;
}

void Material::setCullMode(CullMode cullMode){
	mCullMode = cullMode;
}

void Material::setAmbient(const Color& color){
	mAmbientColor = color;
}

void Material::setDiffuse(const Color& color){
	mDiffuseColor = color;
}

const Color& Material::getAmbient() const{
	return mAmbientColor;
}

const Color& Material::getDiffuse() const{
	return mDiffuseColor;
}

bool Material::isAlphaTested() const{
	return mAlphaTested;
}

CullMode Material::getCullMode() const{
	return mCullMode;
}

} // </wt>

