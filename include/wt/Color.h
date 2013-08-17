#ifndef WT_COLOR_H
#define WT_COLOR_H


#include "wt/ASerializable.h"

namespace wt{

class Color : public lua::ASerializable{
public:
	float red;
	float green;
	float blue;
	float alpha;

	Color(float r=0.0f, float g=0.0f, float b=0.0f, float a=1.0f);

	Color(const glm::vec3& clr);

	Color(const glm::vec4& clr);

	void set(float r, float g, float b, float a);

	static Color fromHSV(float h, float s, float v);

	void serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const;

	void deserialize(lua::State* luaState, const LuaPlus::LuaObject& src);

	static Color random();

	String hex() const;

	static Color transparent();

	static Color Cyan();

	static Color Magenta();

	static Color Yellow();

	static Color Red();

	static Color Green();

	static Color White();

	static Color Black();

	static Color Gray();

	static Color Blue();

	float* getDataPtr();

	const float* getDataPtr() const;

	glm::vec3 asVec3() const;

	glm::vec4 asVec4() const;

}; // </Color>

bool operator==(const Color& a, const Color& b);

bool operator!=(const Color& a, const Color& b);

}; // </wt>

#endif
