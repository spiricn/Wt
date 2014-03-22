#ifndef _H_TEXT_VIEW
#define _H_TEXT_VIEW

#include "wt/stdafx.h"

#include "wt/gui/View.h"

namespace wt
{

namespace gui
{

class TextView : public View{
public:
	enum ScalingMode{
		/**
		 * Scale the text view as the text grow
		 */
		eSCALE_AUTO,

		/**
		 * Do not scale the view, crop the text if necessary 
		 */
		eSCALE_FIXED
	}; // </ScalingMode>

	TextView();

	void setScalingMode(ScalingMode mode);

	void setFontScale(float size);

	void setText(const String& text);

	String getText() const;

	void setTextColor(const Color& color);

	void draw(Canvas& c);
	
private:
	Color mTextColor;
	StringList mLines;
	String mText;
	float mFontScale;
	ScalingMode mScalingMode;
}; // </TextView>

}; // </gui>

}; // </wt>

#endif