#ifndef WT_RENDERER_H
#define WT_RENDERER_H


#include "wt/stdafx.h"

#include "wt/MatrixStack.h"
#include "wt/GLShaderProgram.h"
#include "wt/AGameWindow.h"
#include "wt/Frustum.h"
#include "wt/Scene.h"
#include "wt/FrameBuffer.h"
#include "wt/Color.h"
#include "wt/Utils.h"
#include "wt/Geometry.h"
#include "wt/Material.h"
#include "wt/Mesh.h"
#include "wt/Model.h"
#include <td/td.h>
#include "wt/TerrainChunk.h"
#include "wt/Font.h"
#include "wt/SkeletalAnimationPlayer.h"
#include "wt/RenderTarget.h"
#include "wt/RenderBuffer.h"
#include "wt/ParticleEffect.h"

#include "wt/ARenderer.h"
#include "wt/SkyboxRenderer.h"
#include "wt/EventManager.h"

namespace wt{

class GodRayShader : public gl::ShaderProgram{
public:
	void create(){
		createFromFiles("shaders/godray.vp", "shaders/godray.fp");

		bindAttribLocation(0, "inPosition");
		bindAttribLocation(1, "inTexCoord");

		link();
	}
}; // </GodRayShader>

class RectShader : public gl::ShaderProgram{
public:
	void create(){
		createFromFiles("shaders/rect.vp", "shaders/rect.fp");

		bindAttribLocation(0, "inPosition");
		bindAttribLocation(1, "inTexCoord");

		link();
	}
}; // </RectShader>

#define MATERIALS_DISABLED

class Renderer : public EventListener{
public:
	enum PolygonMode {
		LINE=GL_LINE,
		FILL=GL_FILL,
		POINT=GL_POINT
	};

	enum Culling {BACK_FACE=GL_BACK,
		FRONT_FACE=GL_FRONT,
		NONE
	};

	
	struct RenderState{
		PolygonMode polygonMode;
		Culling culling;
		bool renderSkeletons;

		RenderState() : polygonMode(FILL),
			culling(NONE), renderSkeletons(false){
		}
	};

public:
	Renderer(EventManager* eventManager);

	~Renderer();

	RenderState& getRenderState();

	bool handleEvent(const Sp<Event> evt);

	void attachRenderer(ARenderer* renderer);

	void setRenderBones(bool r);

	void setBoneWidth(float width);

	void setRenderBoundingBoxes(bool state);

	void render(const PxBounds3& bounds, math::Camera* camera, const Color& clr);

	void init(uint32_t portW, uint32_t portH );

	void saveScreenshot(const String& path);

	const math::Frustum& getFrustum() const;
	
	void setFOV(float angle);

	void setClearColor(const Color& clr);

	void setViewPort(uint32_t width, uint32_t height);

	void setPolygoneMode(PolygonMode mode);

	void setFaceCulling(Culling mode);

	void render(Texture2D* tex, const glm::vec2& viewport, float x, float y, float w, float h, const Color& color=Color::white());

	void render(Scene* scene, Model* model, Mesh* mesh, ARenderer::PassType pass);

	/** Render all passes of the scene to the given render target
	 *
	 * @param target	Destination target to which the scene will be rendered.
     *					may be NULL in which case the scene is going to be rendered
	 *					to the default frame bufffer
	 */
	void render(Scene& scene, RenderTarget* target=NULL);

private:
	/** Render entire scene in a single pass */
	void render(Scene& scene, ARenderer::PassType pass);

	/** Render actor's bone and all its predecessors recursivley (used for debugging */
	void Renderer::render(Scene* scene, const ModelledActor* actor, SkeletonBone* bone);

private:
	math::MatrixStack mMatStack;
	Scene* mScene;
	math::Frustum mFrustum;
	Color mClearColor;

	GodRayShader mGodRayShader;
	RectShader mRectShader;
	gl::ShaderProgram mGodraySunShader;

	gl::Batch mFontBatch, mSunBatch;

	GLuint mFontTexture;
	RenderState mRenderState;
	glm::vec2 mViewPort;

	
	typedef std::vector<ARenderer*> RendererList;
	RendererList mSceneRenderers;


	Material mDefaultMaterial;

	gl::Batch mCubeBatch;

	// Shadows
	Texture2D mGodraySunTexture;
	Texture2D mShadowMap, mGodrayPass1, mGodrayPass2;
	gl::FrameBuffer mShadowFBO, mGodrayFBO;
	gl::RenderBuffer mGodrayDepthBuffer;

	gl::Batch mGodrayBatch;

	bool mRenderBones;
	bool mRenderBoundingBoxes;

	float mBoneWidth;

	void setShaderLightUniforms(Scene* scene, gl::ShaderProgram& prog);

	void setShaderMaterialUniforms(Material* material, gl::ShaderProgram& prog);

	uint32_t mNumRenderedTerrainNodes;

	void initGodray();

	EventManager* mEventManager;

}; // </Renderer>

}; // </wt>

#endif
