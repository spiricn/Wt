#ifndef WT_RENDERER_H
#define WT_RENDERER_H


#include "wt/stdafx.h"

#include "wt/MatrixStack.h"
#include "wt/GLShaderProgram.h"
#include "wt/AGameWindow.h"
#include "wt/Scene.h"
#include "wt/FrameBuffer.h"
#include "wt/Color.h"
#include "wt/Utils.h"
#include "wt/Geometry.h"
#include "wt/Material.h"
#include "wt/Mesh.h"
#include "wt/Model.h"
#include <td/td.h>
#include "wt/Font.h"
#include "wt/SkeletalAnimationPlayer.h"
#include "wt/RenderTarget.h"
#include "wt/RenderBuffer.h"
#include "wt/ParticleEffect.h"

#include "wt/ARenderer.h"
#include "wt/SkyboxRenderer.h"
#include "wt/EventManager.h"


namespace wt
{

namespace gui
{
	class Layout;
} // </gui>

class DeferredRender;

class GodRayShader : public gl::ShaderProgram{
public:
	void create();
}; // </GodRayShader>

class RectShader : public gl::ShaderProgram{
public:
	void create();
}; // </RectShader>

#define MATERIALS_DISABLED

class Renderer : public Scene::IListener{
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
	Renderer(EventManager* eventManager, Scene* scene);

	virtual ~Renderer();

	RenderState& getRenderState();

	void attachRenderer(ARenderer* renderer);

	void setRenderBones(bool r);

	void setBoneWidth(float width);

	void setRenderBoundingBoxes(bool state);

	void render(const PxBounds3& bounds, Camera* camera, const Color& clr);

	void render(const ATransformable* tf, Camera* camera);

	void init(uint32_t portW, uint32_t portH );

	void saveScreenshot(const String& path);
	
	void setClearColor(const Color& clr);

	void setViewPort(uint32_t width, uint32_t height);

	void setPolygoneMode(PolygonMode mode);

	void setFaceCulling(Culling mode);

	void render(Texture2D* tex, const glm::vec2& viewport, float x, float y, float w, float h, const Color& color=Color::White(), bool flipVertically=false);

	void render(Scene* scene, Model* model, Mesh* mesh, ARenderer::PassType pass);

	/** Render all passes of the scene to the given render target
	 *
	 * @param target	Destination target to which the scene will be rendered.
     *					may be NULL in which case the scene is going to be rendered
	 *					to the default frame bufffer
	 */
	void render(Scene& scene, RenderTarget* target=NULL);

	void setShaderLightUniforms(Scene* scene, gl::ShaderProgram& prog);

	void setRenderAxes(bool state);

	void onSceneFogParamsChanged(Scene* scene, const FogDesc& desc);

	void onSceneLightUpdated(Scene* scene, const ALight& light);

	void onSceneLightCreated(Scene* scene, const ALight& light);

	void onSceneLightDeleted(Scene* scene, const ALight& light);

private:
	/** Render entire scene in a single pass */
	void render(Scene& scene, Camera& camera, ARenderer::PassType pass);

	/** Render actor's bone and all its predecessors recursivley (used for debugging */
	void Renderer::render(Scene* scene, const ModelledActor* actor, SkeletonBone* bone);

	void godrayPass(Scene& scene);

	void render(Scene& scene, gui::Layout* window);

private:
	struct GodrayTech{
		GodRayShader shader;
		Texture2D defaultSourceTexture;
		gl::Batch sourceBatch;
		gl::Batch quadBatch;
		gl::FrameBuffer frameBuffer;
		Texture2D pass1, pass2;
		gl::RenderBuffer depthBuffer;
		gl::ShaderProgram sourceShader;

		GodrayTech() : pass1(NULL, 0, "", Texture2D::eRECT_TEXTURE), pass2(NULL, 0, "", Texture2D::eRECT_TEXTURE){
		}
	} mGodray;


	struct ShadowMapping{
		Texture2D shadowMap;
	} mShadowMapping;
	typedef std::vector<ARenderer*> RendererList;

	

	void setShaderMaterialUniforms(Material* material, gl::ShaderProgram& prog);

	void initGodray();

	Scene* mScene;
	math::MatrixStack mMatStack;
	Color mClearColor;
	RectShader mRectShader;
	gl::Batch mFontBatch;
	GLuint mFontTexture;
	RenderState mRenderState;
	glm::vec2 mViewPort;	
	RendererList mSceneRenderers;
	Material mDefaultMaterial;
	gl::Batch mCubeBatch;
	bool mRenderBones;
	bool mRenderBoundingBoxes;
	bool mRenderAxes;
	float mBoneWidth;
	DeferredRender* mDeferredRenderer;
	uint32_t mNumRenderedTerrainNodes;
	EventManager* mEventManager;

	

}; // </Renderer>

}; // </wt>

#endif
