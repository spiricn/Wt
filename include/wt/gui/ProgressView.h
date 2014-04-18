#ifndef WT_PROGRESSVIEW_H
#define WT_PROGRESSVIEW_H

#include "wt/stdafx.h"

#include "wt/gui/View.h"

namespace wt
{

namespace gui
{

class ProgressView : public View{
public:
	ProgressView(View* parent, EventManager* eventManager, AGameInput* input);

	void setDrawProgress(bool state);

	void setProgressText(const String& text);

	void setProgress(float progress);

	void draw(ICanvas& c);

private:
	String mProgressText;
	float mProgress;
	bool mDrawProgress;
}; // </ProgressView>

} // </gui>

} // </wt>

#endif // </WT_PROGRESSVIEW_H>
