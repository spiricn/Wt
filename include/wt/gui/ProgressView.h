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
	ProgressView(Layout* parent);

	void setDrawProgress(bool state);

	void setProgress(float progress);

	void draw(Canvas& c);

private:
	float mProgress;
	bool mDrawProgress;
	String mProgressText;
}; // </ProgressView>

} // </gui>

} // </wt>

#endif // </WT_PROGRESSVIEW_H>
