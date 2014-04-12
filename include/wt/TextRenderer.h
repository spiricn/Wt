/**
 * @file TextRenderer.h
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */
 
#ifndef WT_TEXTRENDERER_H
#define WT_TEXTRENDERER_H

#include "wt/GLShaderProgram.h"
#include "wt/GLBatch.h"
#include "wt/Texture2D.h"

namespace wt
{

class TextRenderer : public Singleton<TextRenderer>{
public:
	TextRenderer();

	void draw(Font* font, const String& text, const glm::vec2& viewport, float x, float y, const Color& color, float scale);

private:
	gl::ShaderProgram mShader;
}; // </TextRenderer>

} // </wt>

#endif // </WT_TEXTRENDERER_H>
