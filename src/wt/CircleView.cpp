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
CircleView::CircleView(View* parent, EventManager* eventManager, AGameInput* input) : mRadius(1.0f), mColor(Color::White()), View(parent, eventManager, input){
}

void CircleView::setColor(const Color& color){
	dirty();

	mColor = color;
}

void CircleView::setRadius(float r){
	dirty();

	mRadius = r;
	setSize(glm::vec2(r, r));
}

void CircleView::draw(ICanvas& c){
	Paint paint;
	paint.setStyle( Paint::eSTYLE_FILL );
	paint.setFillColor( mColor );

	c.drawCircle(mRadius/2, mRadius/2, mRadius/2, &paint);
}

} // </gui>
	
} // </wt>

