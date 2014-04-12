/**
 * @file ICanvas.h
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */
 
#ifndef WT_ICANVAS_H
#define WT_ICANVAS_H

#include "wt/stdafx.h"

namespace wt
{

class Texture2D;
class Color;
class Font;

namespace gui
{

class Paint;
class Rect;

class ICanvas{
public:
	virtual ~ICanvas();

	virtual void drawCircle(float posX, float posY, float radius, const Paint* paint=NULL) = 0;

	virtual void drawRect(float posX, float posY, float width, float height, const Paint* paint=NULL) = 0;

	virtual void drawLine(float startX, float startY, float endX, float endY, const Paint* paint=NULL) = 0;

	virtual void drawTexture(Texture2D* texture, float posX, float posY, float width, float height, const Color& color) = 0;

	virtual void drawTextFmt(Font* font, const String& text, const glm::vec2& pos, const glm::vec2& size, const Color& color, float scale) = 0;

	virtual void setSize(uint32_t width, uint32_t height) = 0;

	virtual glm::vec2 getSize() const = 0;

	virtual Texture2D* getTexture() = 0;

	virtual void drawText(Font* font, const String& text, float x, float y, const Color& color, float scale) = 0;

	virtual void setClearColor(const Color& color) = 0;

	virtual void clear() = 0;

	virtual void bind() = 0;

	virtual void drawRect(const Rect& rect, const Paint* paint=NULL);

	virtual void drawLine(const glm::vec2& startPos,  const glm::vec2& endPos, const Paint* paint=NULL);

}; // </ICanvas>

} // </gui>

} // </wt>

#endif // </WT_ICANVAS_H>
