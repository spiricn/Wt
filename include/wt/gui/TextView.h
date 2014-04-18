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

	typedef uint32_t PosFlag;

public:
	static const PosFlag ePOS_CENTER_VERTICAL = 1 << 0;

	static const PosFlag ePOS_CENTER_HORIZONTAL = 1 << 1;

public:

	TextView(View* parent, EventManager* eventManager, AGameInput* input);

	void setTextScaleMode(ScalingMode mode);

	void setPositionFlags(PosFlag flags);

	void setFontScale(float size);

	void setText(const String& text);

	String getText() const;

	void setTextColor(const Color& color);

	void draw(ICanvas& c);
	
private:
	Color mTextColor;
	StringList mLines;
	String mText;
	float mFontScale;
	ScalingMode mScalingMode;
	PosFlag mPosFlags;
}; // </TextView>

}; // </gui>

}; // </wt>

#endif