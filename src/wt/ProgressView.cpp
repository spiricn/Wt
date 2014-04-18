/**
 * @file ProgressView.cpp
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */

#include "wt/stdafx.h"
#include "wt/gui/ProgressView.h"
#include "wt/Utils.h"

#define TD_TRACE_TAG "ProgressView"

namespace wt
{

namespace gui
{

ProgressView::ProgressView(View* parent, EventManager* eventManager, AGameInput* input) : mProgress(0.0f), mDrawProgress(true), mProgressText(""), View(parent, eventManager, input){
}


void ProgressView::setDrawProgress(bool state){
	dirty();

	mDrawProgress = state;
}

void ProgressView::setProgress(float progress){
	if(mProgress != progress){
		mProgress = glm::clamp(progress, 0.0f, 100.0f);
		dirty();
	}
}

void ProgressView::setProgressText(const String& text){
	mProgressText = text;

	dirty();
}

void ProgressView::draw(ICanvas& c){
	WT_ASSERT(getFont(), "No font specified for ProgressView instance");

	Paint paint;

	// Base rect
	paint
		.setStyle( Paint::eSTYLE_FILL )
		.setFillColor( Color(127.0/255.0, 127.0/255.0, 127.0/255.0))
	;
	c.drawRect(0, 0, getSize().x, getSize().y, &paint);

	// Progress rect
	paint.setStyle( Paint::eSTYLE_FILL_AND_STROKE );
	paint.setFillColor( Color(112/255.0, 146/255.0, 190/255.0) );
	paint.setStrokeColor( Color::Black() );
	c.drawRect(0, 0, getSize().x * (mProgress/100.0f), getSize().y, &paint);

	if(mDrawProgress){
		String text = utils::print("%s%.2f%%", mProgressText.c_str(), mProgress);

		glm::vec2 s = getFont()->measureString(text);
		c.drawTextFmt(getFont(), text, getSize()/2.0f - s/2.0f, getSize(), Color::Black(), 1.0f);
	}

	// Background rect
	paint.setStyle( Paint::eSTYLE_STROKE );
	paint.setStrokeColor( Color(195.0/255.0, 195.0/255.0, 195.0/255.0) );
	c.drawRect(0, 0, getSize().x, getSize().y, &paint);
}

} // </gui>
	
} // </wt>
