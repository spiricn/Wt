/**
 * @file RectRenderer.h
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */
 
#ifndef WT_RECTRENDERER_H
#define WT_RECTRENDERER_H

#include "wt/Singleton.h"

#include "wt/GLShaderProgram.h"
#include "wt/GLBatch.h"
#include "wt/Texture2D.h"

namespace wt
{

class RectRenderer : public Singleton<RectRenderer>{
public:
	RectRenderer();

	void draw(const glm::vec2& viewport, const glm::vec2& pos, const glm::vec2& size, Texture2D* texture=NULL, const Color& color=Color::White());

private:
	struct Vertex{
		float x;
		float y;

		float s;
		float t;
	}; // </Vertex>

	enum ShaderAttribs{
		eATTRIB_POSITION,
		eATTRIB_TEX_COORD
	}; // </ShaderAttribs>

private:
	gl::Batch mBatch;
	gl::ShaderProgram mShader;
}; // </RectRenderer>

} // </wt>

#endif // </WT_RECTRENDERER_H>
