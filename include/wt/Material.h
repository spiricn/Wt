#ifndef WT_MATERIAL_H
#define WT_MATERIAL_H

#include "wt/Common.h"
#include "wt/Color.h"

namespace wt
{

class Material{
public:
	Material();

	void setBlend(bool state);

	bool getBlend() const;

	float getShininess() const;

	void setShininess(float shininess);

	const Color& getSpecular() const;

	void setSpecular(const Color& color);

	void setWireframe(bool state);

	bool isWireframe() const;

	void setAlphaTest(bool state);

	void setCullMode(CullMode cullMode);

	void setAmbient(const Color& color);

	void setDiffuse(const Color& color);

	const Color& getAmbient() const;

	const Color& getDiffuse() const;

	bool isAlphaTested() const;

	CullMode getCullMode() const;

private:
	CullMode mCullMode;
	Color mAmbientColor;
	Color mDiffuseColor;
	Color mSpecularColor;
	float mShininess;
	bool mAlphaTested;
	bool mWireframe;
	bool mBlend;

}; // </Material>


} // </wt>

#endif // </WT_MATERIAL_H>