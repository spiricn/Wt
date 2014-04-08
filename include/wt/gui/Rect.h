/**
 * @file Rect.h
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */
 
#ifndef WT_RECT_H
#define WT_RECT_H

#include "wt/stdafx.h"

namespace wt
{

namespace gui
{

class Rect : public glm::vec4{
public:
	Rect(float x=0.0f, float y=0.0f, float w=0.0f, float h=0.0f);

	~Rect();

	bool contains(const glm::vec2& point) const;

	bool overlaps(const Rect& other) const;

	glm::vec2 getTopLeft() const;

	glm::vec2 getTopRight() const;

	glm::vec2 getBottomLeft() const;

	glm::vec2 getBottomRight() const;

	glm::vec2 getPosition() const;

	glm::vec2 getSize() const;

public:
	float& width;
	float& height;

}; // </Rect>

} // </gui>

} // </wt>

#endif // </WT_RECT_H>
