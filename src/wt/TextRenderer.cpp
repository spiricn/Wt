/**
 * @file TextRenderer.cpp
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */

#include "wt/stdafx.h"
#include "wt/TextRenderer.h"
#include "wt/ShaderFactory.h"
#include "wt/Font.h"
#include "wt/FontAtlas.h"

#define TD_TRACE_TAG "TextRenderer"

namespace wt
{

TextRenderer::TextRenderer(){
	ShaderFactory::createShader(mShader, "font.vp", "font.fp");

	mShader.bindAttribLocation(0, "aVertex");	

	mShader.link();

}

void TextRenderer::draw(Font* font, const String& text, const glm::vec2& viewport, float startX, float startY, const Color& color, float scale){
	mShader.use();
	mShader.setUniformVal("uTexture", 0);
	mShader.setUniformVal("uMatProj", glm::ortho(0.0f, viewport.x, viewport.y, 0.0f ));
	

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

	gl::Batch batch;
	batch.create(
		GL_TRIANGLES,
		points, numPoints, sizeof(Point),
		NULL, 0, 0,
		GL_UNSIGNED_INT
		);

	batch.setVertexAttribute(
		0,
			4, GL_FLOAT, 0);

	mShader.use();
	glActiveTexture(GL_TEXTURE0);
	font->getFontAtlas().getTexture()->bind();
	mShader.setUniformVal("uColor", color);

	mShader.setUniformVal("uModelViewMat", glm::mat4(1.0f));

	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	glDisable(GL_DEPTH_TEST);

	// TODO investigate this further, will probably cause some problems
	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
		GL_SRC_ALPHA, 1.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	batch.render();

	delete[] points;
}
	
} // </wt>

