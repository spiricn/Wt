/**
 * @file Paint.cpp
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */

#include "wt/stdafx.h"
#include "wt/gui/Paint.h"

#define TD_TRACE_TAG "Paint"

namespace wt
{

namespace gui
{

Paint::Paint() : mStyle(eSTYLE_FILL_AND_STROKE), mFillColor(Color::White()), mStrokeColor(Color::Black()), mStrokeWidth(1.0f){
}

Paint::Style Paint::getStyle() const{
	return mStyle;
}

float Paint::getStrokeWidth() const{
	return mStrokeWidth;
}

const Color& Paint::getStrokeColor() const{
	return mStrokeColor;
}

const Color& Paint::getFillColor() const{
	return mFillColor;
}

Paint& Paint::setStyle(Style style){
	mStyle = style;

	return *this;
}

Paint& Paint::setFillColor(const Color& color){
	mFillColor = color;

	return *this;
}

Paint& Paint::setStrokeColor(const Color& color){
	mStrokeColor = color;

	return *this;
}

Paint& Paint::getStrokeWidth(float width){
	mStrokeWidth = width;

	return *this;
}

} // </gui>
	
} // </wt>

