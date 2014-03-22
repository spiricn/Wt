#ifndef WT_CANVAS_H
#define WT_CANVAS_H

#include "wt/stdafx.h"
#include "wt/FrameBuffer.h"
#include "wt/GLShaderProgram.h"
#include "wt/Font.h"
#include "wt/GLBatch.h"
#include "wt/ShaderFactory.h"

namespace wt
{

namespace gui
{

using namespace gl;

class FontShader : public wt::gl::ShaderProgram{
public:
	enum Attribs{
		VERTEX=0,
	};

	void create(){
		ShaderFactory::createShader(*this, "font.vp", "font.fp");

		bindAttribLocation(VERTEX, "aVertex");	

		link();
	}
}; // </FontShader>

class Canvas{
public:
	enum FillMode{
		eFILL,
		eLINE
	}; // </FillMode>

	enum RectMode{
		eRECT_FILL=GL_FILL,
		eRECT_LINE=GL_LINE,
	}; // </RectMode>

	glm::vec2 getSize() const;

	void setOutput(Texture2D* output);

	uint32_t getLineWidth(Font* font, const String& str, float maxWidth, float scale);

	void drawTextFmt(Font* font, const String& text,
		const glm::vec2& pos, const glm::vec2& size, const Color& color=Color::Black(), float scale=1.0f);

	void drawText(Font* font, const String& text,
		float x, float y, const Color& color, float scale=1.0f);

	void resize(uint32_t w, uint32_t h);

	void create(uint32_t w, uint32_t h);

	void create(Texture2D* target);

	void setTranslation(const glm::vec2& pos);

	FrameBuffer& getFBO();

	Texture2D* getTexture();

	void clear();

	void drawTexture(Texture2D* texture, float x, float y, float w, float h, const Color& color);

	glm::mat4& getProjMat();

	glm::mat4& getModelViewMat();

	void drawCircle(float x, float y, float radius, const Color& color, FillMode fill = eFILL);

	void drawRect(float x, float y, float w, float h, const Color& color,  RectMode mode=eRECT_FILL);

	void drawLine(float sx, float sy, float ex, float ey, const Color& color, float lineWidth = 1.0f);

protected:
	void create();

private:
	FrameBuffer mFrameBfr;
	uint32_t mWidth, mHeight;
	glm::mat4x4 mModelView;
	glm::mat4x4 mProjMat;
	Texture2D* mTargetTex;
	FontShader mFontShader;
	gl::Batch mFontBatch;

}; // </Canvas>

} // </gui>

} // </wt>

#endif // </WT_CANVAS_H>
