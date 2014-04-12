/**
 * @file RectRenderer.cpp
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */

#include "wt/stdafx.h"
#include "wt/RectRenderer.h"
#include "wt/ShaderFactory.h"

#define TD_TRACE_TAG "RectRenderer"

namespace wt
{

RectRenderer::RectRenderer(){
	ShaderFactory::createShader(mShader, "rect_shader.vp", "rect_shader.fp");

	static int v = 1;

	Vertex vertices[4] = {
		// Top left ( 0 )
		0, 0, 0, 1,

		// Top right ( 1 )
		v, 0, 1, 1,
			
		// Bottom left ( 2 )
		0, v, 0, 0,

		// Bottom right ( 3 )
		v, v, 1, 0,
	};

	uint8_t indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	mBatch.create(GL_TRIANGLES, vertices, 4, sizeof(Vertex), indices, 6, sizeof(uint32_t), GL_UNSIGNED_BYTE);

	// Position
	mBatch.setVertexAttribute(eATTRIB_POSITION, 2, GL_FLOAT, offsetof(Vertex, x));
	mShader.bindAttribLocation(eATTRIB_POSITION, "inPosition");

	// Texture coordinates
	mBatch.setVertexAttribute(eATTRIB_TEX_COORD, 2, GL_FLOAT, offsetof(Vertex, s));
	mShader.bindAttribLocation(eATTRIB_TEX_COORD, "inTexCoord");

	mShader.link();

	mShader.use();

	mShader.setUniformVal("uTextureSampler", 0);
}

void RectRenderer::draw(const glm::vec2& viewport, const glm::vec2& pos, const glm::vec2& size, Texture2D* texture, const Color& color){
	mShader.use();

	if(texture){
		gl( ActiveTexture(GL_TEXTURE0) );
		texture->bind();
	}

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glm::mat4 projMat = glm::ortho(0.0f, viewport.x, 0.0f, viewport.y);
	mShader.setUniformVal("uProjMat", projMat);

	glm::mat4 modelMat = glm::translate(pos.x, pos.y, 0.0f) * glm::scale(size.x, size.y, 1.0f);

	mShader.setUniformVal("uModelViewMat", modelMat);

	mShader.setUniformVal("uTextured", texture ? true : false);

	mShader.setUniformVal("uColor", color);

	mBatch.render();
}

	
} // </wt>

