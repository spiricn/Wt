/**
 * @file ProgressView.cpp
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */

#include "wt/stdafx.h"
#include "wt/gui/ProgressView.h"

#define TD_TRACE_TAG "ProgressView"

namespace wt
{

namespace gui
{

ProgressView::ProgressView(Layout* parent) : mProgress(0.0f), mDrawProgress(true), mProgressText(""), View(parent){
}


void ProgressView::setDrawProgress(bool state){
	dirty();

	mDrawProgress = state;
}

void ProgressView::setProgress(float progress){
	if(mProgress != progress){
		mProgress = progress;
		dirty();
	}
}

void ProgressView::draw(Canvas& c){
	WT_ASSERT(getFont(), "No font specified for ProgressView instance");

	Canvas::Paint paint;

	// Base rect
	paint.style = Canvas::Paint::eSTYLE_FILL;
	paint.fillColor = Color(127.0/255.0, 127.0/255.0, 127.0/255.0);
	c.drawRect(0, 0, getSize().x, getSize().y, &paint);

	// Progress rect
	paint.strokeColor = Canvas::Paint::eSTYLE_FILL_AND_STROKE;
	paint.fillColor = Color(112/255.0, 146/255.0, 190/255.0);
	paint.strokeColor = Color::Black();
	c.drawRect(0, 0, getSize().x * (mProgress/100.0f), getSize().y, &paint);

	if(mDrawProgress){
		String text = utils::print("%.2f%%", mProgress);

		glm::vec2 s = getFont()->measureString(text);
		c.drawTextFmt(getFont(), text, getSize()/2.0f - s/2.0f, getSize(), Color::Black());
	}

	// Background rect
	paint.style = Canvas::Paint::eSTYLE_STROKE;
	paint.strokeColor = Color(195.0/255.0, 195.0/255.0, 195.0/255.0);
	c.drawRect(0, 0, getSize().x, getSize().y, &paint);
}

} // </gui>
	
} // </wt>
