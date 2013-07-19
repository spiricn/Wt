#ifndef WT_CANVAS_H
#define WT_CANVAS_H


#include "wt/stdafx.h"
#include "wt/FrameBuffer.h"
#include "wt/GLShaderProgram.h"
#include "wt/Font.h"
#include "wt/GLBatch.h"

namespace wt{



namespace Gui{

using namespace Gl;

class FontShader : public wt::Gl::ShaderProgram{
public:
	enum Attribs{
		VERTEX=0,
	};

	void create(){
		createFromFiles(
			"shaders/font.vp",
			"shaders/font.fp");

		bindAttribLocation(VERTEX, "aVertex");	

		link();
	}
};

class Canvas{
private:
	FrameBuffer mFrameBfr;
	uint32_t mWidth, mHeight;
	glm::mat4x4 mModelView;
	glm::mat4x4 mProjMat;
	Texture2D* mTargetTex;
	FontShader mFontShader;
	Gl::Batch mFontBatch;

protected:
	void create(){
		mFrameBfr.create();

		mFrameBfr.bind(FrameBuffer::DRAW);

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

public:
	enum FillMode{
		eFILL,
		eLINE
	}; // </FillMode>

	glm::vec2 getSize() const{
		return glm::vec2(mWidth, mHeight);
	}

	void setOutput(Texture2D* output){
		mFrameBfr.addAttachment(GL_COLOR_ATTACHMENT0, output==NULL ? mTargetTex : output);
	}

	uint32_t getLineWidth(Font* font, const String& str, float maxWidth){

		String bfr;
		char t[2] = {0, 0};
		float w=0;
		for(uint32_t i=0; i<str.size(); i++){
			if(str[i] == '\n'){
				return i==0 ? 0 : i;
			}

			t[0] = str[i];
			bfr.append(t);

			w = font->measureString(bfr).x;
			if(w > maxWidth){
				return i-1;
			}
		}

		return str.size();
	}


	void drawTextFmt(Font* font, const String& text,
		const glm::vec2& pos, const glm::vec2& size, const Color& color=Color::black(), float scale=1.0f){

		String bfr = text;

		
		uint32_t lineHeight = ((font->getFace()->bbox.yMax-font->getFace()->bbox.yMin)>>6)/2; //(font->getFace()->height>>6);
		uint32_t y = 0;


		if(lineHeight > size.y){
			// No text can fit in this area
			return;
		}

		while(!bfr.empty()){

			if(!bfr.empty() && bfr[0] == '\n'){
				// Skip new lines
				bfr = bfr.substr(1);
				continue;
			}

			uint32_t lineEnd = getLineWidth(font, bfr, size.x);
			if(lineEnd == 0){
				break;
			}

			String lineText = bfr.substr(0, lineEnd);
			bfr = bfr.substr(lineEnd);

			drawText(font, lineText, pos.x, pos.y + y*lineHeight, color, scale);
			y++;

			if( (y + 1)*lineHeight >= size.y){
				return;
			}
		}
	}

	void drawText(Font* font, const String& text,
		float x, float y, const Color& color, float scale=1.0f){


		float offset = font->measureString(text).y;

		struct Point{
			float x, y, s, t;
			Point(float _x=0, float _y=0, float _s=0, float _t=0) : x(_x), y(_y), s(_s), t(_t){
			}
		};


		// Vertices & indices
		uint32_t numPoints = 6*text.size();
		Point* points = new Point[numPoints];
		uint32_t* indices = new uint32_t[numPoints];

		for(uint32_t i=0; i<numPoints; i++){
			indices[i] = i;
		}

		uint32_t p=0;

		float texWidth = font->mAtlas.getTexture()->getWidth();
		float texHeight = font->mAtlas.getTexture()->getHeigth();

		for(uint32_t i=0; i<text.size(); i++){
			const FontAtlas::CharInfo& info = font->mAtlas.getCharInfo( text[i] );
			float x2 = x + info.mBl;
			float y2 = (y - info.mBt)+offset;
			float w = info.mBw;
			float h = info.mBh;

			x += info.mAx;
			y += info.mAy;


			// First triangle
			points[p++] = Point(x2, y2,
				info.mTx, 0);

			points[p++] = Point(x2+w, y2,
				info.mTx + (info.mBw/texWidth), 0);

			points[p++] = Point(x2+w, (y2+h),
				info.mTx + (info.mBw/texWidth), info.mBh/texHeight);

			// Second triangle
			points[p++] = Point(x2, y2,
				info.mTx, 0);

			points[p++] = Point(x2+w, (y2+h),
				info.mTx + (info.mBw/texWidth), info.mBh/texHeight);

			points[p++] = Point(x2, (y2+h),
				info.mTx, info.mBh/texHeight);
		}


		mFontBatch.create(
			GL_TRIANGLES,
			points, numPoints, sizeof(Point),
			indices, numPoints, sizeof(uint32_t),
			GL_UNSIGNED_INT
			);

		mFontShader.use();
		font->mAtlas.getTexture()->bind();
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
		delete[] indices;
	}

	void resize(uint32_t w, uint32_t h){
		mWidth = w;
		mHeight = h;

		mProjMat = glm::ortho(0.0f, (float)mWidth, 0.0f, (float)mHeight);
		mFontShader.use();
		mFontShader.setUniformVal("uMatProj", mProjMat);

		mTargetTex->bind();
		mTargetTex->setData(mWidth, mHeight, GL_RGBA, GL_RGBA8, NULL, GL_FLOAT);
	}

	void create(uint32_t w, uint32_t h){
		mWidth = w;
		mHeight = h;
		mTargetTex = new Texture2D();
		create();
	}

	void create(Texture2D* target){
		mWidth = target->getWidth();
		mHeight = target->getHeigth();
		mTargetTex = target;
		create();
	}

	void setTranslation(const glm::vec2& pos){
		mModelView = glm::translate(pos.x, pos.y, 0.0f);
	}

	FrameBuffer& getFBO(){
		return mFrameBfr;
	}

	Texture2D* getTexture(){
		return mTargetTex;
	}

	void clear(){
		mFrameBfr.bind(FrameBuffer::DRAW);

		gl( Disable(GL_BLEND) );
		//mTargetTex->bind();

		static GLenum bfrMap[] = {GL_COLOR_ATTACHMENT0};
		gl( DrawBuffers(1, bfrMap) );

		gl( ClearColor(0.0, 0.0, 0.0, 0.0) );
		gl( Clear(GL_COLOR_BUFFER_BIT) );
	}

	void drawTexture(Texture2D* texture, float x, float y, float w, float h, const Color& color){
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
		glColor4f(color.mRed, color.mGreen, color.mBlue, color.mAlpha);
		glVertex2f(x,	y);		glTexCoord2f(1.0, 0.0);
		glVertex2f(x+w, y);		glTexCoord2f(1.0, 1.0);
		glVertex2f(x+w, y+h);	glTexCoord2f(0.0, 1.0);
		glVertex2f(x,	y+h);	glTexCoord2f(0.0, 0.0);

		glEnd();
	}

	glm::mat4& getProjMat(){
		return mProjMat;
	}

	glm::mat4& getModelViewMat(){
		return mModelView;
	}

	enum RectMode{
		eRECT_FILL=GL_FILL,
		eRECT_LINE=GL_LINE,
	};

	void drawCircle(float x, float y, float radius, const Color& color, FillMode fill = eFILL){
		glUseProgram(0);

		// blending
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		
		glColor4f(color.mRed, color.mGreen, color.mBlue, color.mAlpha);

		glBegin(fill == eFILL ? GL_TRIANGLE_FAN : GL_LINE_LOOP);

		for(float angle=0.0f; angle<360.0f; angle+=5.0f){
			glVertex2f(x + glm::sin(angle) * radius, y + glm::cos(angle) * radius);
		}
		glEnd();
	}

	void drawRect(float x, float y, float w, float h, const Color& color,  RectMode mode=eRECT_FILL){
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

		glColor4f(color.mRed, color.mGreen, color.mBlue, color.mAlpha);

		glBegin(GL_QUADS);
			glVertex2f(x + 0, y + 0);
			glVertex2f(x + w, (y) + 0);
			glVertex2f(x + w, (y) + h);
			glVertex2f(x + 0, (y) + h);
		glEnd();

		glFlush();
	}

	void drawLine(float sx, float sy, float ex, float ey, const Color& color, float lineWidth = 1.0f){
		gl( Disable(GL_BLEND) );

		gl( Enable(GL_LINE_SMOOTH) );

		gl( LineWidth(lineWidth) );
		gl( BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );

		gl( Disable(GL_DEPTH_TEST) );
		gl( Disable(GL_TEXTURE_2D) );
		gl( Color4f(color.mRed, color.mGreen, color.mBlue, color.mAlpha) );
		gl( PolygonMode(GL_FRONT_AND_BACK, GL_FILL) );

		gl( Begin(GL_LINES) );
			gl( Vertex2f(sx, sy) );
			gl( Vertex2f(ex, ey) );
		gl( End() );

		gl( Flush() );
	}

};


}; // </Gui>

}; // </wt>



#endif
