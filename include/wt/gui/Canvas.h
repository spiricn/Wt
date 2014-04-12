#ifndef WT_CANVAS_H
#define WT_CANVAS_H

#include "wt/stdafx.h"
#include "wt/FrameBuffer.h"
#include "wt/GLShaderProgram.h"
#include "wt/Font.h"
#include "wt/GLBatch.h"
#include "wt/ShaderFactory.h"
#include "wt/gui/ICanvas.h"
#include "wt/gui/Paint.h"
#include "wt/Color.h"

namespace wt
{

namespace gui
{

class Canvas : public ICanvas{
public:
	Canvas(uint32_t width=1, uint32_t height=1);

	glm::vec2 getSize() const;

	uint32_t getLineWidth(Font* font, const String& str, float maxWidth, float scale);

	void drawTextFmt(Font* font, const String& text, const glm::vec2& pos, const glm::vec2& size, const Color& color=Color::Black(), float scale=1.0f);

	void drawText(Font* font, const String& text, float x, float y, const Color& color, float scale=1.0f);

	void setSize(uint32_t w, uint32_t h);

	gl::FrameBuffer& getFBO();

	Texture2D* getTexture();

	void clear();

	void drawTexture(Texture2D* texture, float x, float y, float w, float h, const Color& color);

	void setClearColor(const Color& color);

	void drawCircle(float x, float y, float radius, const Paint* paint=NULL);

	void drawRect(float x, float y, float w, float h, const Paint* paint=NULL);

	void drawLine(float sx, float sy, float ex, float ey, const Paint* paint=NULL);

	void bind();

private:
	gl::FrameBuffer mFrameBfr;
	uint32_t mWidth;
	uint32_t mHeight;
	Texture2D* mTargetTex;
	Color mClearColor;
	Paint mDefaultPaint;
}; // </Canvas>

} // </gui>

} // </wt>

#endif // </WT_CANVAS_H>
