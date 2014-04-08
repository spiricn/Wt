#ifndef WT_CIRCLEVIEW_H
#define WT_CIRCLEVIEW_H

#include "wt/gui/View.h"

namespace wt
{

namespace gui
{

class CircleView : public View{
public:
	CircleView(Layout* parent);

	void setColor(const Color& color);

	void setRadius(float r);

	void draw(Canvas& c);

private:
	float mRadius;
	Color mColor;

}; // </CircleView>

} // </gui>

} // </wt>

#endif // </WT_CIRCLEVIEW_H>