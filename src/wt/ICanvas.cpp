/**
 * @file ICanvas.cpp
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */

#include "wt/stdafx.h"
#include "wt/gui/ICanvas.h"
#include "wt/gui/Rect.h"

#define TD_TRACE_TAG "ICanvas"

namespace wt
{

namespace gui
{

ICanvas::~ICanvas(){
}

void ICanvas::drawRect(const Rect& rect, const Paint* paint){
	drawRect(rect.x, rect.y, rect.width, rect.height, paint);
}

void ICanvas::drawLine(const glm::vec2& startPos,  const glm::vec2& endPos, const Paint* paint){
	drawLine(startPos.x, startPos.y, endPos.x, endPos.y, paint);
}

} // </gui>

} // </wt>

