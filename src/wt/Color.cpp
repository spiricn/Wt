#include "wt/stdafx.h"

#include "wt/Color.h"
#include "wt/Exception.h"
#include "wt/Math.h"

namespace wt{

Color::Color(float r, float g, float b, float a){
		set(r, g, b, a);
}

void Color::set(float r, float g, float b, float a){
	mRed=r;
	mGreen=g;
	mBlue=b;
	mAlpha=a;
}

Color Color::transparent(){
	return Color(0, 0, 0, 0);
}


String Color::hex() const{
	char bfr[9];
	sprintf(bfr, "%02x%02x%02x%02x", (Uint8)(mRed*255), (Uint8)(mGreen*255), (Uint8)(mBlue*255), (Uint8)(mAlpha*255));

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

	color.mRed += v - c;
	color.mGreen += v - c;
	color.mBlue += v - c;

	return color;
}

void Color::serialize(LuaPlus::LuaObject& dst){
	dst.SetNumber("r", mRed);
	dst.SetNumber("g", mGreen);
	dst.SetNumber("b", mBlue);
	dst.SetNumber("a", mAlpha);
}

void Color::deserialize(const LuaPlus::LuaObject& src){
	#ifdef WT_CHECKED
	WT_ASSERT(
		src.Get("r").IsNumber() &&
		src.Get("g").IsNumber() &&
		src.Get("b").IsNumber() &&
		src.Get("a").IsNumber(), "Invalid color table");
	#endif

	mRed = src.Get("r").ToNumber();
	mGreen = src.Get("g").ToNumber();
	mBlue = src.Get("b").ToNumber();
	mAlpha = src.Get("a").ToNumber();
}

float* Color::getDataPtr(){
	return &mRed;
}

const float* Color::getDataPtr() const{
	return &mRed;
}

Color Color::cyan(){
	return Color(0.0, 1.0, 1.0);
}

Color Color::random(){
	return Color(math::random(),  math::random(),  math::random());
}

Color Color::magenta(){
	return Color(1.0, 0.0, 1.0);
}

Color Color::yellow(){
	return Color(1.0, 1.0, 0.0);
}

Color Color::red(){
	return Color(1.0f);
}

Color Color::green(){
	return Color(0.0f, 1.0f);
}

Color Color::white(){
	return Color(1.0f, 1.0f, 1.0f);
}

Color Color::black(){
	return Color();
}

Color Color::gray(){
	return Color(0.5f, 0.5f, 0.5f);
}

Color Color::blue(){
	return Color(0.0f, 0.0f, 1.0f);
}


}; // </wt>
