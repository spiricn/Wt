#ifndef _H_PROGRESS_VIEW
#define _H_PROGRESS_VIEW

#include "wt/stdafx.h"

#include "wt/Gui/View.h"

namespace wt{

namespace Gui{

class ProgressView : public View{
private:
	float mProgress;
	bool mDrawProgress;
	String mProgressText;

public:

	ProgressView() : mProgress(0.0f), mDrawProgress(true), mProgressText(""){
	}


	void setDrawProgress(bool state){ DIRTY
		mDrawProgress = state;
	}

	void setProgress(float progress){
		if(mProgress != progress){
			mProgress = progress;
			DIRTY
		}
	}

	void draw(Canvas& c){
		WT_ASSERT(getFont(), "No font specified for ProgressView instance");

		// base rect
		c.drawRect(0, 0,
			getSize().x, getSize().y, Color(127.0/255.0, 127.0/255.0, 127.0/255.0));

		// progress rect
		c.drawRect(0, 0,
			getSize().x * (mProgress/100.0f), getSize().y, Color(181.0/255.0, 230.0/255.0, 29.0/255.0));

		// border rect
		c.drawRect(0, 0,
			getSize().x, getSize().y, Color(195.0/255.0, 195.0/255.0, 195.0/255.0), Canvas::eRECT_LINE);

		if(mDrawProgress){
			char bfr[64];

			sprintf(bfr, "%.2f%%", mProgress);

			glm::vec2 s = getFont()->measureString(bfr);
			c.drawTextFmt(getFont(), bfr, getSize()/2.0f - s/2.0f, getSize(), Color::black());
		}
	}

}; // </ProgressView>

}; // </Gui>

}; // </wt>

#endif