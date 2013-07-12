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
#include "wt/Log.h"
#include "wt/TerrainChunk.h"
#include "wt/Font.h"
#include "wt/SkeletalAnimationPlayer.h"
#include "wt/RenderTarget.h"

namespace wt{


class SkyboxShader : public wt::Gl::ShaderProgram{
public:
	void create(){
		createFromFiles("shaders/skybox.vp", "shaders/skybox.fp");

		bindAttribLocation(0, "inVertex");
		bindAttribLocation(1, "inTexCoord");

		link();
	}
};

class TerrainShader : public wt::Gl::ShaderProgram{
public:
	void create(){
		createFromFiles("shaders\\terrain.vp",
			"shaders\\terrain.fp");

		bindAttribLocation(0, "inPosition");
		bindAttribLocation(1, "inTexture");
		bindAttribLocation(2, "inTiledTexture");
		bindAttribLocation(3, "inNormal");

		link();

		use();
		setUniformVal("texArray", 0);
		setUniformVal("map", 1);
	}

	void setMVPMatrix(const glm::mat4x4& model, const glm::mat4x4& view, const glm::mat4x4& projection){
		setUniformVal("uModelMat", model);
		setUniformVal("uViewMat",view);
		setUniformVal("uProjMat", projection);
	}

	void setTileTextures(TextureArray* textures){
		// texture tiles
		glActiveTexture(GL_TEXTURE0);
		textures->bind();

	/*	Texture::Params tileParams;
		tileParams.wrapS = tileParams.wrapT = GL_REPEAT;
		Texture::setParams(GL_TEXTURE_2D, tileParams);
		Texture::setParams(GL_TEXTURE_2D_ARRAY, tileParams);*/
	}

	void setColorMap(Texture2D* map){
		glActiveTexture(GL_TEXTURE1);
		map->bind();

		//Texture::Params mapParams;
		//Texture::setParams(GL_TEXTURE_2D, mapParams);
		//Texture::setParams(GL_TEXTURE_2D_ARRAY, mapParams);
	}


};

class BasicShader : public wt::Gl::ShaderProgram{
public:
	BasicShader(){
	}
	enum Streams{
		VERTEX = 0,
		TEXTURE_COORDS = 1,
		NORMALS = 2,
		TANGENT = 3,
		BONE_IDS = 4,
		BONE_WEIGHTS = 5
	};

	void create(){
		createFromFiles("shaders\\basic.vp",
			"shaders\\basic.fp");//, "shaders/basic.gp");

		bindAttribLocation(VERTEX, "inVertex");
		bindAttribLocation(TEXTURE_COORDS, "inTexCoord");
		bindAttribLocation(NORMALS, "inNormals");
		bindAttribLocation(TANGENT, "inTangent");
		bindAttribLocation(BONE_IDS, "inBoneIds");
		bindAttribLocation(BONE_WEIGHTS, "inWeights");

		link();

		use();
		setUniformVal("uSampler", 0);
		setUniformVal("uNormalMap", 1);
	}


	void setNormalMap(Texture2D* map){
		setUniformVal("uUseNormalMap", map?1:0);

		if(map!=NULL){
			glActiveTexture(GL_TEXTURE1);
			map->bind();
		}

	}

	void setModelMatrix(const glm::mat4& model){
		setUniformVal("uModelMat", model);
	}

	void setMPMatrix(const glm::mat4x4& view, const glm::mat4x4& projection){
		setUniformVal("uViewMat",view);
		setUniformVal("uProjMat", projection);
	}
};


class GodRayShader : public Gl::ShaderProgram{
public:
	void create(){
		createFromFiles("shaders/godray.vp", "shaders/godray.fp");

		bindAttribLocation(0, "inPosition");
		bindAttribLocation(1, "inTexCoord");

		link();
	}
}; // </GodRayShader>

class RectShader : public Gl::ShaderProgram{
public:
	void create(){
		createFromFiles("shaders/rect.vp", "shaders/rect.fp");

		bindAttribLocation(0, "inPosition");
		bindAttribLocation(1, "inTexCoord");

		link();
	}
}; // </RectShader>

#define MATERIALS_DISABLED

class Renderer{
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

	struct GodRayParams{
		glm::vec3 mSunPos;
		Color mSunColor;
		Color mRayColor;
		float mSunSize;
		float mExposure;
		float mDecay;
		float mDensity;
		float mWeight;
		int mNumSamples;
		bool mIsEnabled;

		GodRayParams() : mSunPos(glm::vec3(0.347047, 0.283466, 0.893994)*1000.0f), mSunColor(Color::white()),
			mRayColor(Color(0.1, 0.1, 0.1, 1.0)), mSunSize(30.0f),  mExposure(0.0034f),
			mDecay(1.0f), mDensity(0.84f), mWeight(5.65f), mNumSamples(100), mIsEnabled(false){
		}
	};

private:
	static const char* TAG;

	struct RenderState{
		PolygonMode polygonMode;
		Culling culling;
		bool renderSkeletons;

		RenderState() : polygonMode(FILL),
			culling(NONE), renderSkeletons(false){
		}
	};

	

	math::MatrixStack mMatStack;
	Scene* mScene;
	math::Frustum mFrustum;
	Color mClearColor;
	GodRayParams mGodRayParams;

	// Shaders
	BasicShader mBasicShader;
	TerrainShader mTerrainShader;
	SkyboxShader mSkyShader;
	GodRayShader mGodRayShader;
	RectShader mRectShader;
	Gl::ShaderProgram mGodraySunShader;

	Gl::Batch mFontBatch, mSunBatch;

	GLuint mFontTexture;
	RenderState mRenderState;
	glm::vec2 mViewPort;

	
	Texture2D mInvalidTex;
	Material mDefaultMaterial;

	Gl::Batch mCubeBatch;

	// Shadows
	Texture2D mGodraySunTexture;
	Texture2D mShadowMap, mGodrayPass1, mGodrayPass2;
	Gl::FrameBuffer mShadowFBO, mGodrayFBO;

	Gl::Batch mGodrayBatch;

	bool mRenderBones;
	float mBoneWidth;

	void setShaderLightUniforms(Scene* scene, Gl::ShaderProgram& prog);

	void setShaderMaterialUniforms(Material* material, Gl::ShaderProgram& prog);

	Uint32 mNumRenderedTerrainNodes;

	enum PassType{
		eNORMAL_PASS,
		eGODRAY_PASS,
		eSHADOW_PASS
	};

	void initGodray();

public:
	Renderer();

	RenderState& getRenderState();

	void setRenderBones(bool r){
		mRenderBones = r;
	}

	void setBoneWidth(float width){
		mBoneWidth = width;
	}

	void render(const PxBounds3& bounds, math::Camera* camera, const Color& clr);

	void init(Uint32 portW, Uint32 portH );

	void saveScreenshot(const String& path);

	const math::Frustum& getFrustum() const;
	
	void setFOV(float angle){
		mFrustum.setPerspectiveProj(
			mViewPort.x, mViewPort.y, angle, mFrustum.getNearClip(), mFrustum.getFarClip()
			);
	}

	void setClearColor(const Color& clr);

	void setViewPort(Uint32 width, Uint32 height);

	void setPolygoneMode(PolygonMode mode);

	void setFaceCulling(Culling mode);

	void render(Texture2D* tex, const glm::vec2& viewport, float x, float y, float w, float h, const Color& color=Color::white());

	void render(Scene* scene, const float frustum[6][4], TerrainNode* chunk);	

	void render(Scene& scene, SkyBox* sky);

	void renderTerrainBounds(TerrainNode* node, math::Camera* camera);

	void render(Scene* scene,  SceneActor* actor, SkeletonBone* bone);

	void draw(Scene& scene, PassType pass);

	void render(Scene* scene, SceneActor* actor, PassType pass);

	void render(Scene* scene, Model* model, Mesh* mesh, PassType pass);

	void render(Scene* scene, const float frustum[6][4], Terrain* terrain, PassType pass);

	void getGodRayParams(GodRayParams& dst);

	void setGodRayParams(const GodRayParams& src);

	///
	void render(Scene& scene, RenderTarget* target=NULL);
}; // </Renderer>

}; // </wt>

#endif
