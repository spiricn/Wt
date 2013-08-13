#ifndef WT_DEFERREDRENDERER_H
#define WT_DEFERREDRENDERER_H

#include "wt/FrameBuffer.h"
#include "wt/GLShaderProgram.h"
#include "wt/GLBatch.h"

namespace wt
{

namespace math
{
	class Camera;
}

class Scene;

class DeferredRender{
public:
	enum TextureType{
		eGTEX_INVALID = -1,

		eGTEX_POSITION = 0,
		eGTEX_DIFFUSE,
		eGTEX_NORMAL,

		eGTEX_MAX
	};

	enum LightPassShader{
		eLIGHT_SHADER_NONE=-1,

		eLIGHT_SHADER_DIRECTIONAL = 0,
		eLIGHT_SHADER_POINT,

		eLIGHT_SHADER_MAX,
	};

	DeferredRender(uint32_t width, uint32_t height);

	void resize(uint32_t width, uint32_t height);

	void bindForGeometryPass();

	void debugBlit(const glm::vec2& screenSize);

	void bindForLightPass();

	void bindForFinalPass();

	void directionalLightPass(Scene* scene, math::Camera* camera);

	void bindForStencilPass();

	void stencilPass(Scene* scene, math::Camera* camera, uint32_t pointLightIndex);

	void pointLightPass(Scene* scene, math::Camera* camera, uint32_t pointLightIndex);

	void startFrame();

	gl::ShaderProgram* getLightShader(LightPassShader shader);

	gl::FrameBuffer* getFrameBuffer();

private:
	Texture2D mTextures[eGTEX_MAX];
	Texture2D mDepthTexture;
	Texture2D mFinalTexture;

	gl::FrameBuffer mFrameBuffer;
	uint32_t mWidth, mHeight;

	gl::ShaderProgram mLightShaders[eLIGHT_SHADER_MAX];
	gl::ShaderProgram mStencilPassShader;

	gl::Batch mQuadBatch;
	gl::Batch mSphereBatch;

}; // </DeferredRender>

} // </wt>

#endif // </WT_DEFERREDRENDERER_H>