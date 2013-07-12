#ifndef WT_COLOR_H
#define WT_COLOR_H


#include "wt/ASerializable.h"

namespace wt{

class Color : public Lua::ASerializable{
public:
	float mRed, mGreen, mBlue, mAlpha;

	Color(float r=0.0f, float g=0.0f, float b=0.0f, float a=1.0f);

	void set(float r, float g, float b, float a);

	static Color fromHSV(float h, float s, float v);

	void serialize(LuaPlus::LuaObject& dst);

	void deserialize(const LuaPlus::LuaObject& src);

	static Color random();

	String hex() const;

	static Color transparent();

	static Color cyan();

	static Color magenta();

	static Color yellow();

	static Color red();

	static Color green();

	static Color white();

	static Color black();

	static Color gray();

	static Color blue();

	float* getDataPtr();

	const float* getDataPtr() const;

}; // </Color>

}; // </wt>

#endif
