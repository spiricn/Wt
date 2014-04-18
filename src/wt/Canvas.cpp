#include "wt/stdafx.h"

#include "wt/gui/Canvas.h"
#include "wt/FontAtlas.h"
#include "wt/Font.h"
#include "wt/RectRenderer.h"
#include "wt/TextRenderer.h"

namespace wt
{

namespace gui
{

using namespace gl;

Canvas::Canvas(uint32_t width, uint32_t height) : mWidth(width), mHeight(height){
	mClearColor = Color(0, 0, 0, 0);

	mFrameBfr.create();

	mFrameBfr.bind(FrameBuffer::eMODE_DRAW);

	mTargetTex = new Texture2D();

	mTargetTex->create();
	mTargetTex->bind();
	mTargetTex->setData(mWidth, mHeight, GL_RGBA, GL_RGBA8, NULL, GL_FLOAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	mFrameBfr.addAttachment(GL_COLOR_ATTACHMENT0, mTargetTex);
}

void Canvas::bind(){
	gl( Viewport(0, 0, mWidth, mHeight) );

	getFBO().bind(FrameBuffer::eMODE_DRAW);

	getFBO().unbind(FrameBuffer::eMODE_READ);

	static GLenum bfrMap[] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, bfrMap);
}

glm::vec2 Canvas::getSize() const{
	return glm::vec2(mWidth, mHeight);
}

uint32_t Canvas::getLineWidth(Font* font, const String& str, float maxWidth, float scale){
	String bfr;
	char t[2] = {0, 0};

	float w=0;

	// Iterate through entire string
	for(uint32_t i=0; i<str.size(); i++){
		if(str[i] == '\n'){
			return i==0 ? 0 : i;
		}

		// Append character after character to bfr
		t[0] = str[i];
		bfr.append(t);

		// Measure the new buffer
		w = font->measureString(bfr, scale).x;
		if(w > maxWidth){
			// We've reached the end of the width limit
			return i-1;
		}
	}

	return str.size();
}


void Canvas::drawTextFmt(Font* font, const String& text,
	const glm::vec2& pos, const glm::vec2& size, const Color& color, float scale){

	String bfr = text;

	// Height, in pixels, of a single line of text
	const float lineHeight = font->getLineHeight() * scale;

	if(lineHeight > size.y){
		// No text can fit in this area
		return;
	}

	uint32_t y = 0;

	while(!bfr.empty()){
		if(!bfr.empty() && bfr[0] == '\n'){
			// Skip new lines
			bfr = bfr.substr(1);
			continue;
		}

		// Number of charactes that can fit in this line
		uint32_t lineEnd = getLineWidth(font, bfr, size.x, scale);
		if(lineEnd == 0){
			break;
		}

		// Get the current line string
		String lineText = bfr.substr(0, lineEnd);
		
		// Remove it from the buffer
		bfr = bfr.substr(lineEnd);

		// Render the line
		drawText(font, lineText, pos.x, pos.y + y*lineHeight, color, scale);

		y++;

		if( (y + 1)*lineHeight >= size.y){
			return;
		}
	}
}

void Canvas::drawText(Font* font, const String& text, float startX, float startY, const Color& color, float scale){
	TextRenderer::getSingleton().draw(font, text, glm::vec2(mWidth, mHeight), startX, startY, color, scale);
}

void Canvas::setSize(uint32_t w, uint32_t h){
	mWidth = w;
	mHeight = h;

	mTargetTex->bind();
	mTargetTex->setData(mWidth, mHeight, GL_RGBA, GL_RGBA8, NULL, GL_FLOAT);
}

FrameBuffer& Canvas::getFBO(){
	return mFrameBfr;
}

Texture2D* Canvas::getTexture(){
	return mTargetTex;
}

void Canvas::clear(){
	mFrameBfr.bind(FrameBuffer::eMODE_DRAW);

	gl( Disable(GL_BLEND) );
	//mTargetTex->bind();

	static GLenum bfrMap[] = {GL_COLOR_ATTACHMENT0};
	gl( DrawBuffers(1, bfrMap) );

	gl( ClearColor(mClearColor.red, mClearColor.green, mClearColor.blue, mClearColor.alpha) );
	gl( Clear(GL_COLOR_BUFFER_BIT) );
}

void Canvas::drawTexture(Texture2D* texture, float x, float y, float w, float h, const Color& color){
	gl( Enable(GL_BLEND) );
	gl( BlendEquation(GL_FUNC_ADD) );
	gl( BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );
	gl( Disable(GL_DEPTH_TEST) );
	gl( Disable(GL_CULL_FACE) );
	gl( PolygonMode(GL_FRONT_AND_BACK, GL_FILL) );

	RectRenderer::getSingleton().draw(glm::vec2(mWidth, mHeight), glm::vec2(x, y), glm::vec2(w, h), texture, color);
}

void Canvas::drawCircle(float x, float y, float radius, const Paint* paint){
	if(!paint){
		paint = &mDefaultPaint;
	}

	glUseProgram(0);

	// blending
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	
	if(paint->getStyle() == Paint::eSTYLE_FILL){
		glColor4f(paint->getFillColor().red, paint->getFillColor().green, paint->getFillColor().blue, paint->getFillColor().alpha);
	}
	else if(paint->getStyle() == Paint::eSTYLE_STROKE){
		glColor4f(paint->getStrokeColor().red, paint->getStrokeColor().green, paint->getStrokeColor().blue, paint->getStrokeColor().alpha);
	}
	else{
		TRACEW("Not implemented");
	}

	glBegin(paint->getStyle() == Paint::eSTYLE_FILL ? GL_TRIANGLE_FAN : GL_LINE_LOOP);

	for(float angle=0.0f; angle<360.0f; angle+=5.0f){
		glVertex2f(x + glm::sin(angle) * radius, y + glm::cos(angle) * radius);
	}

	glEnd();
}

void Canvas::drawRect(float x, float y, float w, float h, const Paint* paint){
	if(!paint){
		paint = &mDefaultPaint;
	}
#if 1
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	if(paint->getStyle() == Paint::eSTYLE_FILL || paint->getStyle() == Paint::eSTYLE_FILL_AND_STROKE){
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		RectRenderer::getSingleton().draw(glm::vec2(mWidth, mHeight), glm::vec2(x, y), glm::vec2(w, h), NULL, paint->getFillColor());
	}
	
	if(paint->getStyle() == Paint::eSTYLE_STROKE || paint->getStyle() == Paint::eSTYLE_FILL_AND_STROKE){
		// Draw stroke
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		gl( LineWidth(paint->getStrokeWidth()) );

		const float o = paint->getStrokeWidth()/2.0f;

		RectRenderer::getSingleton().draw(
			glm::vec2(mWidth, mHeight),
			glm::vec2(x, y) + glm::vec2(o, o), glm::vec2(w, h) - 4.0f * glm::vec2(o, o), NULL, paint->getStrokeColor());
	}

#else
	// State setup
	glUseProgram(0);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);

	if(paint->getStyle() == Paint::eSTYLE_FILL || paint->getStyle() == Paint::eSTYLE_FILL_AND_STROKE){
		// Draw fill
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glColor4f(paint->getFillColor().red, paint->getFillColor().green, paint->getFillColor().blue, paint->getFillColor().alpha);

		glBegin(GL_QUADS);
		glVertex2f(x + 0, y + 0);
		glVertex2f(x + w, (y) + 0);
		glVertex2f(x + w, (y) + h);
		glVertex2f(x + 0, (y) + h);
		glEnd();
		glFlush();
	}

	if(paint->getStyle() == Paint::eSTYLE_STROKE || paint->getStyle() == Paint::eSTYLE_FILL_AND_STROKE){
		// Draw stroke
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glColor4f(paint->getStrokeColor().red, paint->getStrokeColor().green, paint->getStrokeColor().blue, paint->getStrokeColor().alpha);

		gl( LineWidth(paint->getStrokeWidth()) );

		const float o = paint->getStrokeWidth()/2.0f;

		glBegin(GL_QUADS);
		glVertex2f(x + o, y + o);
		glVertex2f(x + w - o, y + o);
		glVertex2f(x + w - o, y + h - o);
		glVertex2f(x + o, y + h - o);
		glEnd();
		glFlush();
	}
#endif
}

void Canvas::drawLine(float sx, float sy, float ex, float ey, const Paint* paint){
	if(!paint){
		paint = &mDefaultPaint;
	}

	gl( Disable(GL_BLEND) );

	gl( Enable(GL_LINE_SMOOTH) );

	gl( LineWidth(paint->getStrokeWidth()) );
	gl( BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );

	gl( Disable(GL_DEPTH_TEST) );
	gl( Disable(GL_TEXTURE_2D) );
	gl( Color4f(paint->getStrokeColor().red, paint->getStrokeColor().green, paint->getStrokeColor().blue, paint->getStrokeColor().alpha) );
	gl( PolygonMode(GL_FRONT_AND_BACK, GL_FILL) );

	glBegin(GL_LINES);
		glVertex2f(sx, sy);
		glVertex2f(ex, ey);
	glEnd();

	gl( Flush() );
}

void Canvas::setClearColor(const Color& color){
	mClearColor = color;
}

} // </gui>

} // </wt>