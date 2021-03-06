#include "wt/stdafx.h"

#include "wt/Color.h"
#include "wt/Exception.h"
#include "wt/Math.h"

#define TD_TRACE_TAG "Color"

namespace wt{

Color::Color(float r, float g, float b, float a){
		set(r, g, b, a);
}

void Color::set(float r, float g, float b, float a){
	red=r;
	green=g;
	blue=b;
	alpha=a;
}

Color::Color(const glm::vec3& clr) : red(clr.r), green(clr.g), blue(clr.b), alpha(1.0f){
}

Color::Color(const glm::vec4& clr)  : red(clr.r), green(clr.g), blue(clr.b), alpha(clr.a){
}

Color Color::transparent(){
	return Color(0, 0, 0, 0);
}


String Color::hex() const{
	char bfr[9];
	sprintf(bfr, "%02x%02x%02x%02x", (uint8_t)(red*255), (uint8_t)(green*255), (uint8_t)(blue*255), (uint8_t)(alpha*255));

	return bfr;
}
/**
* converts hue/saturation/value to RGB color representation
* values are in range [0.0, 1.0]
*/
Color Color::fromHSV(float h, float s, float v){
	float c = v * s;
	h = fmod((h * 6.0), 6.0);
	float x = c * (1.0 - abs(fmod(h, 2.0f) - 1.0));
	Color color;
	float a=1.0f;
	if (0.0 <= h && h < 1.0) {
		color = Color(c, x, 0.0, a);
	} else if (1.0 <= h && h < 2.0) {
		color = Color(x, c, 0.0, a);
	} else if (2.0 <= h && h < 3.0) {
		color = Color(0.0, c, x, a);
	} else if (3.0 <= h && h < 4.0) {
		color = Color(0.0, x, c, a);
	} else if (4.0 <= h && h < 5.0) {
		color = Color(x, 0.0, c, a);
	} else if (5.0 <= h && h < 6.0) {
		color = Color(c, 0.0, x, a);
	} else {
		color = Color(0.0, 0.0, 0.0, a);
	}

	color.red += v - c;
	color.green += v - c;
	color.blue += v - c;

	return color;
}

void Color::serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const{
	dst.SetNumber("r", red);
	dst.SetNumber("g", green);
	dst.SetNumber("b", blue);
	dst.SetNumber("a", alpha);
}

void Color::deserialize(lua::State* luaState, const LuaPlus::LuaObject& src){
	#ifdef WT_CHECKED
	WT_ASSERT(
		src.Get("r").IsNumber() &&
		src.Get("g").IsNumber() &&
		src.Get("b").IsNumber() &&
		src.Get("a").IsNumber(), "Invalid color table");
	#endif

	red = src.Get("r").ToNumber();
	green = src.Get("g").ToNumber();
	blue = src.Get("b").ToNumber();
	alpha = src.Get("a").ToNumber();
}

float* Color::getDataPtr(){
	return &red;
}

const float* Color::getDataPtr() const{
	return &red;
}

Color Color::Cyan(){
	return Color(0.0, 1.0, 1.0);
}

Color Color::random(){
	return Color(math::random(),  math::random(),  math::random());
}

Color Color::Magenta(){
	return Color(1.0, 0.0, 1.0);
}

Color Color::Yellow(){
	return Color(1.0, 1.0, 0.0);
}

Color Color::Red(){
	return Color(1.0f);
}

Color Color::Green(){
	return Color(0.0f, 1.0f);
}

Color Color::White(){
	return Color(1.0f, 1.0f, 1.0f);
}

Color Color::Black(){
	return Color();
}

Color Color::Gray(){
	return Color(0.5f, 0.5f, 0.5f);
}

Color Color::Blue(){
	return Color(0.0f, 0.0f, 1.0f);
}

glm::vec3 Color::asVec3() const{
	return glm::vec3(red, green, blue);
}

glm::vec4 Color::asVec4() const{
	return glm::vec4(red, green, blue, alpha);
}

bool operator==(const Color& a, const Color& b){
	return a.red == b.red && a.green == b.green && a.blue == b.blue && a.alpha == b.alpha;
}

bool operator!=(const Color& a, const Color& b){
	return !(a == b);
}

}; // </wt>
