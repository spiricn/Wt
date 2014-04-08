/**
 * @file CircleView.cpp
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */

#include "wt/stdafx.h"
#include "wt/gui/CircleView.h"

#define TD_TRACE_TAG "CircleView"

namespace wt
{

namespace gui
{
CircleView::CircleView(Layout* parent) : mRadius(1.0f), mColor(Color::White()), View(parent){
}

void CircleView::setColor(const Color& color){
	dirty();

	mColor = color;
}

void CircleView::setRadius(float r){
	dirty();

	mRadius = r;
	setSize(r, r);
}

void CircleView::draw(Canvas& c){
	Canvas::Paint paint;
	paint.style = Canvas::Paint::eSTYLE_FILL;
	paint.fillColor = mColor;

	c.drawCircle(mRadius/2, mRadius/2, mRadius/2, &paint);
}

} // </gui>
	
} // </wt>

