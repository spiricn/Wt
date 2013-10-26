#ifndef WT_CIRCLEVIEW_H
#define WT_CIRCLEVIEW_H

#include "wt/gui/View.h"

namespace wt{

namespace gui{

class CircleView : public View{
private:
	float mRadius;
	Color mColor;

public:

	CircleView() : mRadius(1.0f), mColor(Color::White()){
	}

	void setColor(const Color& color){
		DIRTY;

		mColor = color;
	}

	void setRadius(float r){
		DIRTY;

		mRadius = r;
		setSize(r, r);
	}

	void draw(Canvas& c){
		c.drawCircle(mRadius/2, mRadius/2, mRadius/2, mColor);
	}
};

}; // </gui>

}; // </wt>

#endif // </WT_CIRCLEVIEW_H>