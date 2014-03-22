#include "wt/stdafx.h"

#include "wt/gui/Canvas.h"
#include "wt/FontAtlas.h"

namespace wt
{

namespace gui
{

using namespace gl;

void Canvas::create(){
	mFrameBfr.create();

	mFrameBfr.bind(FrameBuffer::eMODE_DRAW);

	mTargetTex->create();
	mTargetTex->bind();
	mTargetTex->setData(mWidth, mHeight, GL_RGBA, GL_RGBA8, NULL, GL_FLOAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	setOutput(mTargetTex);

	mProjMat = glm::ortho(0.0f, (float)mWidth, 0.0f, (float)mHeight);

	setTranslation(glm::vec2(0, 0));

	mFontShader.create();
	mFontShader.use();
	mFontShader.setUniformVal("uMatProj", mProjMat);
	mFontShader.setUniformVal("uTexture", 0);

	mFontBatch.create(0, 0, 0, 0, 0, 0);
	mFontBatch.setVertexAttribute(
		FontShader::VERTEX,
			4, GL_FLOAT, 0);
}


glm::vec2 Canvas::getSize() const{
	return glm::vec2(mWidth, mHeight);
}

void Canvas::setOutput(Texture2D* output){
	mFrameBfr.addAttachment(GL_COLOR_ATTACHMENT0, output==NULL ? mTargetTex : output);
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
	const float lineHeight = ( ((font->getFace()->bbox.yMax-font->getFace()->bbox.yMin)>>6)/2 ) * scale;

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
#if 0
	// Debug text bounding box
	drawRect(startX, startY, font->measureString(text, scale).x, font->measureString(text, scale).y, Color::Green(), eRECT_LINE);
#endif
	// Height of the text string provided
	const float maxY = font->measureString(text, scale).y;

	struct Point{
		float x;
		float y;
		float s;
		float t;
		Point(float x=0, float y=0, float s=0, float t=0) : x(x), y(y), s(s), t(t){
		}
	}; // </Point>


	// Vertices & indices
	uint32_t numPoints = 6*text.size();
	Point* points = new Point[numPoints];

	uint32_t vertexCnt = 0;

	const float atlasWidth = font->getFontAtlas().getTexture()->getWidth();
	const float atlasHeight = font->getFontAtlas().getTexture()->getHeigth();

	float currX = startX;
	float currY = startY;

	for(uint32_t i=0; i<text.size(); i++){
		// Atlas info of the current character we're drawing
		const FontAtlas::CharInfo& info = font->getFontAtlas().getCharInfo( text[i] );

		// Absolulte coordinates of the character texture
		const float x = currX + info.bearingX * scale;
		const float y = (currY - info.bearingY * scale) + maxY;

		// Size of the character texture
		const float w = info.width * scale;
		const float h = info.height * scale;

		// First triangle

		// Top left
		points[vertexCnt++] = Point(x, y, info.coord.left, info.coord.top);

		// Top right
		points[vertexCnt++] = Point(x+w, y, info.coord.right, info.coord.top);

		// Bottom right
		points[vertexCnt++] = Point(x+w, y+h, info.coord.right, info.coord.bottom);

		// Second triangle
		
		// Top left
		points[vertexCnt++] = Point(x, y, info.coord.left, info.coord.top);

		// Bottom right
		points[vertexCnt++] = Point(x+w, y+h, info.coord.right, info.coord.bottom);

		// Bottom left
		points[vertexCnt++] = Point(x, y+h, info.coord.left, info.coord.bottom);

		// Advance to the next character
		currX += info.advanceX * scale;
		currY += info.advanceY * scale;
	}


	mFontBatch.create(
		GL_TRIANGLES,
		points, numPoints, sizeof(Point),
		NULL, 0, 0,
		GL_UNSIGNED_INT
		);

	mFontShader.use();
	font->getFontAtlas().getTexture()->bind();
	mFontShader.setUniformVal("uColor", color);
		
	mFontShader.setUniformVal("uModelViewMat", mModelView );

	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	glDisable(GL_DEPTH_TEST);

	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	mFontBatch.render();

	delete[] points;
}

void Canvas::resize(uint32_t w, uint32_t h){
	mWidth = w;
	mHeight = h;

	mProjMat = glm::ortho(0.0f, (float)mWidth, 0.0f, (float)mHeight);
	mFontShader.use();
	mFontShader.setUniformVal("uMatProj", mProjMat);

	mTargetTex->bind();
	mTargetTex->setData(mWidth, mHeight, GL_RGBA, GL_RGBA8, NULL, GL_FLOAT);
}

void Canvas::create(uint32_t w, uint32_t h){
	mWidth = w;
	mHeight = h;
	mTargetTex = new Texture2D();
	create();
}

void Canvas::create(Texture2D* target){
	mWidth = target->getWidth();
	mHeight = target->getHeigth();
	mTargetTex = target;
	create();
}

void Canvas::setTranslation(const glm::vec2& pos){
	mModelView = glm::translate(pos.x, pos.y, 0.0f);
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

	gl( ClearColor(0.0, 0.0, 0.0, 0.0) );
	gl( Clear(GL_COLOR_BUFFER_BIT) );
}

void Canvas::drawTexture(Texture2D* texture, float x, float y, float w, float h, const Color& color){
	glUseProgram(0);

	// bind texture
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture->getTexHandle());

	// setup render state
	glEnable(GL_BLEND);
		
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_COLOR_MATERIAL);
	// draw textured quad
	glBegin(GL_QUADS);
	glColor4f(color.red, color.green, color.blue, color.alpha);
	glVertex2f(x,	y);		glTexCoord2f(1.0, 0.0);
	glVertex2f(x+w, y);		glTexCoord2f(1.0, 1.0);
	glVertex2f(x+w, y+h);	glTexCoord2f(0.0, 1.0);
	glVertex2f(x,	y+h);	glTexCoord2f(0.0, 0.0);

	glEnd();
}

glm::mat4& Canvas::getProjMat(){
	return mProjMat;
}

glm::mat4& Canvas::getModelViewMat(){
	return mModelView;
}


void Canvas::drawCircle(float x, float y, float radius, const Color& color, FillMode fill){
	glUseProgram(0);

	// blending
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
		
	glColor4f(color.red, color.green, color.blue, color.alpha);

	glBegin(fill == eFILL ? GL_TRIANGLE_FAN : GL_LINE_LOOP);

	for(float angle=0.0f; angle<360.0f; angle+=5.0f){
		glVertex2f(x + glm::sin(angle) * radius, y + glm::cos(angle) * radius);
	}
	glEnd();
}

void Canvas::drawRect(float x, float y, float w, float h, const Color& color,  RectMode mode){
	glUseProgram(0);

	// blending
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

	glPolygonMode(GL_FRONT_AND_BACK,
		mode
		);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);

	glColor4f(color.red, color.green, color.blue, color.alpha);

	glBegin(GL_QUADS);
		glVertex2f(x + 0, y + 0);
		glVertex2f(x + w, (y) + 0);
		glVertex2f(x + w, (y) + h);
		glVertex2f(x + 0, (y) + h);
	glEnd();

	glFlush();
}

void Canvas::drawLine(float sx, float sy, float ex, float ey, const Color& color, float lineWidth){
	gl( Disable(GL_BLEND) );

	gl( Enable(GL_LINE_SMOOTH) );

	gl( LineWidth(lineWidth) );
	gl( BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );

	gl( Disable(GL_DEPTH_TEST) );
	gl( Disable(GL_TEXTURE_2D) );
	gl( Color4f(color.red, color.green, color.blue, color.alpha) );
	gl( PolygonMode(GL_FRONT_AND_BACK, GL_FILL) );

	glBegin(GL_LINES);
		glVertex2f(sx, sy);
		glVertex2f(ex, ey);
	glEnd();

	gl( Flush() );
}

} // </gui>

} // </wt>