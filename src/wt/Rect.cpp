/**
 * @file Rect.cpp
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */

#include "wt/stdafx.h"
#include "wt/gui/Rect.h"

#define TD_TRACE_TAG "Rect"

namespace wt
{

namespace gui
{

Rect::Rect(float posX, float posY, float width, float height) : glm::vec4(posX, posY, width, height), width(z), height(w){
}

Rect::~Rect(){
}

bool Rect::contains(const glm::vec2& point) const{
	return point.x >= x && point.x <= x + width && point.y >= y && point.y <= y + height;
}

bool Rect::overlaps(const Rect& other) const{
	return other.contains(getTopLeft()) || other.contains(getTopRight()) ||
		other.contains(getBottomLeft()) || other.contains(getBottomRight());
}

glm::vec2 Rect::getTopLeft() const{
	return glm::vec2(x, y);
}

glm::vec2 Rect::getTopRight() const{
	return glm::vec2(x + width, y);
}

glm::vec2 Rect::getBottomLeft() const{
	return glm::vec2(x, y + height);
}

glm::vec2 Rect::getBottomRight() const{
	return glm::vec2(x + width, y + height);
}

glm::vec2 Rect::getPosition() const{
	return glm::vec2(x, y);
}

glm::vec2 Rect::getSize() const{
	return glm::vec2(width, height);
}

} // </gui>
	
} // </wt>

