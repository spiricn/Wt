/**
 * @file Paint.h
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */
 
#ifndef WT_PAINT_H
#define WT_PAINT_H

#include "wt/Color.h"

namespace wt
{

namespace gui
{

class Paint{
public:
	enum Style{
		eSTYLE_FILL,
		eSTYLE_STROKE,
		eSTYLE_FILL_AND_STROKE
	}; // </Style>

public:

	Paint();

	Style getStyle() const;

	float getStrokeWidth() const;

	const Color& getStrokeColor() const;

	const Color& getFillColor() const;

	Paint& setStyle(Style style);

	Paint& setFillColor(const Color& color);

	Paint& setStrokeColor(const Color& color);

	Paint& getStrokeWidth(float width);

private:
	Color mFillColor;
	Color mStrokeColor;
	float mStrokeWidth;
	Style mStyle;
}; // </Paint>

} // </gui>

} // </wt>

#endif // </WT_PAINT_H>
