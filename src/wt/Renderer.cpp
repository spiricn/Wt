#include "wt/stdafx.h"

#include "wt/Renderer.h"
#include "wt/DevilImageLoader.h"
#include "wt/RenderBuffer.h"
#include "wt/Singleton.h"
#include "wt/gui/Layout.h"
#include "wt/Interpolator.h"
#include "wt/ParticleRenderer.h"
#include "wt/ModelRenderer.h"
#include "wt/TerrainRenderer.h"
#include "wt/DeferredRenderer.h"
#include "wt/ShaderFactory.h"
#include "wt/Scene.h"
#include "wt/Sound.h"
#include "wt/ColliderActor.h"
#include "wt/ModelledActor.h"
#include "wt/ParticleEffect.h"
#include "wt/SkyBox.h"
#include "wt/Terrain.h"
#include "wt/gui/WindowManager.h"
#include "wt/RectRenderer.h"
#include "wt/Texture2DLoader.h"

#define TD_TRACE_TAG "Renderer"



namespace wt
{

class DependencyChecker : public Singleton<DependencyChecker>{
private:
	static const char* kDEPENDENCIES;
public:

	void check(){
		std::stringstream ss(kDEPENDENCIES);

		LOGV("Checking dependencies...");

		while(ss.good()){
			String s;

			ss >> s;
			if(!s.size()){
				continue;
			}

			bool supported = glewIsSupported(s.c_str()) ;

			if(supported){
				LOGV("%s ... OK", s.c_str());
			}
			else{
				LOGE("%s ... MISSING", s.c_str());
			}
		}

		LOGV("Dependencies checked...");
	}
	
}; // </DependencyChecker>

const char* DependencyChecker::kDEPENDENCIES = "GL_VERSION_3_3 GL_ARB_point_sprite GL_ARB_fragment_program GL_ARB_vertex_program";

Renderer::Renderer(EventManager* eventManager, Scene* scene) : mClearColor(0.5, 0, 0, 1.0f),
	mRenderBones(false), mBoneWidth(3.0f), mRenderBoundingBoxes(false), mEventManager(eventManager),
	mDeferredRenderer(NULL), mRenderAxes(false), mScene(scene){

#if 0
	mEventManager->registerListener(this, SceneLightUpdated::TYPE);
	mEventManager->registerListener(this, SceneLightDeleted::TYPE);
#else
	mScene->registerListener(this);
#endif
}

void Renderer::init(uint32_t portW, uint32_t portH ){
	WT_ASSERT(portW > 0 && portH > 0, "Invalid viewport size (%d x %d)", portW, portH);

	LOGV("Initializing...");

	LOGV("GL_VERSION = %s", glGetString(GL_VERSION));
	LOGV("GL_VENDOR = %s", glGetString(GL_VENDOR));
	LOGV("GL_RENDERER = %s", glGetString(GL_RENDERER));
	LOGV("GL_SHADING_LANGUAGE_VERSION = %s", glGetString( GL_SHADING_LANGUAGE_VERSION ));

	mMatStack.pushIdentity();

	

	gl( Enable(GL_DEPTH_TEST) );

	setFaceCulling(BACK_FACE);

	setPolygoneMode(FILL);

	GLenum r = glewInit();
	
	DependencyChecker::getSingleton().check();

	//WT_ASSERT( glewIsSupported("GL_VERSION_3_3"), "Unsupported version of OpenGL");

	LOGV("Initializing glew");
	if(r != GLEW_OK){
		WT_THROW("Error initializing glew \"%s\"", (const char*)glewGetErrorString(r));
	}

	LOGV("Initializing deferred renderer");
	mDeferredRenderer = new DeferredRender(portW, portH);
	mScene->registerListener(mDeferredRenderer);

	// TODO Cause an initial fog update using this hack (do this some other way)
	mScene->setFogDesc( mScene->getFogDesc() );

	setViewPort(portW, portH);

	 //Attach the renderers (order matters)
	attachRenderer(new TerrainRenderer);
	attachRenderer(new ModelRenderer);
	attachRenderer(new SkyboxRenderer);
	attachRenderer(new ParticleRenderer);

	LOGV("Compiling godray shader ...");
	mGodray.shader.create();

	LOGV("Compiling rect shader ...");
	mRectShader.create();
	

	ShaderFactory::createShader(mGodray.sourceShader, "godraysun.vp", "godraysun.fp");
	mGodray.sourceShader.bindAttribLocation(0, "inPosition");
	mGodray.sourceShader.link();



	utils::makeCube(mCubeBatch, 0.01,
		Model::eATTRIB_POSITION, Model::eATTRIB_TEXCOORD, Model::eATTRIB_NORMAL);

	LOGV("Generating default texture");
	gl( GenTextures(1, &mFontTexture) );
	gl( BindTexture(GL_TEXTURE_2D, mFontTexture) );
	gl( TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE) );
	gl( TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE) );
 
	gl( TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) );
	gl( TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) );

	initGodray();

	{
		// godray sun
		float vertices[] = {0,0,0};
		uint32_t indices[] = {0};
		mGodray.sourceBatch.create(
			GL_POINTS,
			vertices, 1, sizeof(float)*3,
			indices, 1, sizeof(uint32_t)
		);

		mGodray.sourceBatch.setVertexAttribute(0, 3, GL_FLOAT, 0);
	}
	setClearColor(Color(0.3, 0.3, 0.3));


	// TODO acquire this from elsewhere
	FileIOStream stream("d:\\Documents\\prog\\c++\\workspace\\Wt\\rsrc\\godray_sun.png", AIOStream::eMODE_READ);
	Texture2DLoader::getSingleton().load(&stream, &mGodray.defaultSourceTexture);

	// Shadow mapping
	mShadowMapping.shadowMap.create();
	mShadowMapping.shadowMap.bind();
	gl( TexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL) );
	gl( TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) );
	gl( TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) );
	gl( TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE) );
	gl( TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE) );


	LOGV("Initialized OK");
}

Renderer::~Renderer(){
	for(RendererList::iterator iter=mSceneRenderers.begin(); iter!=mSceneRenderers.end(); iter++){
		delete *iter;
	}

	mSceneRenderers.clear();

	mScene->unregisterListener(this);
	mScene->unregisterListener(mDeferredRenderer);

	delete mDeferredRenderer;

	// TODO additional cleanup
}

void Renderer::attachRenderer(ARenderer* renderer){
	renderer->create();
	mSceneRenderers.push_back(renderer);
}

void Renderer::onSceneFogParamsChanged(Scene* scene, const FogDesc& desc){
}

void Renderer::onSceneLightUpdated(Scene* scene, const ALight& light){
}

void Renderer::onSceneLightCreated(Scene* scene, const ALight& light){
}

void Renderer::onSceneLightDeleted(Scene* scene, const ALight& light){
}

void Renderer::initGodray(){
	{ // Godray setup
		
		if(mGodray.frameBuffer.isCreated()){
			mGodray.frameBuffer.destroy();

			mGodray.depthBuffer.destroy();
		}

		// Depth buffer
		mGodray.depthBuffer.create();
		mGodray.depthBuffer.setStorage(GL_DEPTH_COMPONENT, mViewPort.x, mViewPort.y);

		mGodray.frameBuffer.create();

		mGodray.frameBuffer.bind(gl::FrameBuffer::eMODE_DRAW);

		mGodray.pass1.create();
		mGodray.pass1.bind();
		gl( PixelStorei(GL_UNPACK_ALIGNMENT, 1) );

		float portW = mViewPort.x;
		float portH = mViewPort.y;
		mGodray.pass1.setData(portW, portH,
			GL_RGBA, GL_RGBA, NULL, GL_UNSIGNED_BYTE, false);

		mGodray.pass2.create();
		mGodray.pass2.bind();
		gl( PixelStorei(GL_UNPACK_ALIGNMENT, 1) );
		mGodray.pass2.setData(portW, portH,
			GL_RGBA, GL_RGBA, NULL, GL_UNSIGNED_BYTE, false);

		mGodray.frameBuffer.addAttachment(GL_COLOR_ATTACHMENT0, &mGodray.pass1);
		mGodray.frameBuffer.addAttachment(GL_COLOR_ATTACHMENT1, &mGodray.pass2);
		mGodray.frameBuffer.addAttachment(GL_DEPTH_ATTACHMENT, mGodray.depthBuffer);

		WT_ASSERT(mGodray.frameBuffer.isComplete(), "Godray FBO incomplete");

		struct vertex{
			float x,y;
			float s,t;
		};

		vertex vertices[] = {
			{0, 0, 0, 0}, {portW, 0, portW, 0}, {portW, portH, portW, portH}, {0, portH, 0, portH}
		};

		uint32_t indices[4] = {0, 1, 2, 3};

		mGodray.quadBatch.destroy();
		mGodray.quadBatch.create(
			GL_QUADS,
			vertices, 4, sizeof(vertex),
			indices, 4, sizeof(uint32_t)
		);

		// inPosition
		mGodray.quadBatch.setVertexAttribute(0, 2, GL_FLOAT, offsetof(vertex, x));
		// inTexCoord
		mGodray.quadBatch.setVertexAttribute(1, 2, GL_FLOAT, offsetof(vertex, s));
	}
}

void extractFrustum(float frustum[6][4], const glm::mat4& view, const glm::mat4& proj){
		const float* clip = glm::value_ptr( (proj*view) );
		float t;

	 /* Extract the numbers for the RIGHT plane */
	   frustum[0][0] = clip[ 3] - clip[ 0];
	   frustum[0][1] = clip[ 7] - clip[ 4];
	   frustum[0][2] = clip[11] - clip[ 8];
	   frustum[0][3] = clip[15] - clip[12];
	 /* Normalize the result */
	   t = sqrt( frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2]    * frustum[0][2] );
	   frustum[0][0] /= t;
	   frustum[0][1] /= t;
	   frustum[0][2] /= t;
	   frustum[0][3] /= t;
	 /* Extract the numbers for the LEFT plane */
	   frustum[1][0] = clip[ 3] + clip[ 0];
	   frustum[1][1] = clip[ 7] + clip[ 4];
	   frustum[1][2] = clip[11] + clip[ 8];
	   frustum[1][3] = clip[15] + clip[12];
	 /* Normalize the result */
	   t = sqrt( frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2]    * frustum[1][2] );
	   frustum[1][0] /= t;
	   frustum[1][1] /= t;
	   frustum[1][2] /= t;
	   frustum[1][3] /= t;
	 /* Extract the BOTTOM plane */
	   frustum[2][0] = clip[ 3] + clip[ 1];
	   frustum[2][1] = clip[ 7] + clip[ 5];
	   frustum[2][2] = clip[11] + clip[ 9];
	   frustum[2][3] = clip[15] + clip[13];
	 /* Normalize the result */
	   t = sqrt( frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2]    * frustum[2][2] );
	   frustum[2][0] /= t;
	   frustum[2][1] /= t;
	   frustum[2][2] /= t;
	   frustum[2][3] /= t;
	 /* Extract the TOP plane */
	   frustum[3][0] = clip[ 3] - clip[ 1];
	   frustum[3][1] = clip[ 7] - clip[ 5];
	   frustum[3][2] = clip[11] - clip[ 9];
	   frustum[3][3] = clip[15] - clip[13];
	 /* Normalize the result */
	   t = sqrt( frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2]    * frustum[3][2] );
	   frustum[3][0] /= t;
	   frustum[3][1] /= t;
	   frustum[3][2] /= t;
	   frustum[3][3] /= t;
	 /* Extract the FAR plane */
	   frustum[4][0] = clip[ 3] - clip[ 2];
	   frustum[4][1] = clip[ 7] - clip[ 6];
	   frustum[4][2] = clip[11] - clip[10];
	   frustum[4][3] = clip[15] - clip[14];
	 /* Normalize the result */
	   t = sqrt( frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2]    * frustum[4][2] );
	   frustum[4][0] /= t;
	   frustum[4][1] /= t;
	   frustum[4][2] /= t;
	   frustum[4][3] /= t;
	 /* Extract the NEAR plane */
	   frustum[5][0] = clip[ 3] + clip[ 2];
	   frustum[5][1] = clip[ 7] + clip[ 6];
	   frustum[5][2] = clip[11] + clip[10];
	   frustum[5][3] = clip[15] + clip[14];
	 /* Normalize the result */
	   t = sqrt( frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2]    * frustum[5][2] );
	   frustum[5][0] /= t;
	   frustum[5][1] /= t;
	   frustum[5][2] /= t;
	   frustum[5][3] /= t;
}

bool pointInFrustum(const float frustum[6][4], const PxVec3& point){
  int p;
  for( p = 0; p < 6; p++ )
    if( frustum[p][0] * point.x + frustum[p][1] * point.y + frustum[p][2] * point.z + frustum[p][3] <= 0 ){
      return false;
	}
  return true;
}

bool inFrustum(const float frustum[6][4], const physx::PxBounds3& bounds){
	PxVec3 c = bounds.getCenter();
	PxVec3 e = bounds.getExtents();

	#define test(point) do{if(pointInFrustum(frustum, point)) return true;}while(0)

	// lower front left
	test(PxVec3(c.x - e.x, c.y - e.y, c.z + e.z));

	// lower front right
	test(PxVec3(c.x + e.x, c.y - e.y, c.z + e.z));

	// upper front right
	test(PxVec3(c.x + e.x, c.y + e.y, c.z + e.z));

	// upper front left
	test(PxVec3(c.x - e.x, c.y + e.y, c.z + e.z));


	// lower back left
	test(PxVec3(c.x - e.x, c.y - e.y, c.z - e.z));

	// lower back right
	test(PxVec3(c.x + e.x, c.y - e.y, c.z - e.z));

	// upper back right
	test(PxVec3(c.x + e.x, c.y + e.y, c.z - e.z));

	// upper back left
	test(PxVec3(c.x - e.x, c.y + e.y, c.z - e.z));

	#undef test

	return false;
}

void saveDepthTexture(Texture2D* texture, const String& path){
	texture->bind();
	Buffer<float> p;
	uint32_t w=texture->getWidth();
	uint32_t h=texture->getHeigth();
	p.create(w*h);
	gl( GetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, (GLvoid*)p.getData()) );

	Buffer<unsigned char> rgb;
	rgb.create(w*h*3);
	for(uint32_t i=0; i<w*h; i++){
		uint8_t depth = (uint8_t)((1.0 - (1.0 - p[i]) * 25.0)*255.0);

		rgb[i*3 + 0] = depth;
		rgb[i*3 + 1] = depth;
		rgb[i*3 + 2] = depth;
	}

	Image img;
	img.setData(w, h, Image::RGB, 3, (unsigned char*)rgb.getData());

	FileIOStream stream(path, AIOStream::eMODE_WRITE);
	DevilImageLoader::getSingleton().save(&stream, &img);
}

void Renderer::render(Scene* scene, const ModelledActor* actor, SkeletonBone* bone){

	// Use fixed pipeline
	gl( UseProgram(0) );

	gl( Disable(GL_DEPTH_TEST) );
	
	// Setup projection matrix
	gl( MatrixMode(GL_PROJECTION) );
	gl( LoadIdentity() );
	gl( LoadMatrixf(glm::value_ptr(scene->getCamera().getProjectionMatrix())) );

	glm::mat4 tmp;
	glm::vec3 p;

	
	// Setup modelview matrix
	glm::mat4 view;
	glm::mat4 model;
	((ModelledActor*)actor)->getTransformable()->getTransformMatrix(model);
	scene->getCamera().getCameraMatrix(view);
	gl( MatrixMode(GL_MODELVIEW) );
	gl( LoadMatrixf(glm::value_ptr(view*model)) );


	GLint currentLineWidth;
	glGetIntegerv(GL_LINE_WIDTH, &currentLineWidth);

	gl( LineWidth(mBoneWidth) );

	if(bone->getParent() != NULL){
		glBegin(GL_LINES);
			glm::mat4 tmp = actor->getAnimationPlayer()->getGlobalBoneTransform(bone->getIndex());
			math::extractTranslation(tmp, p);

			glColor3f(1.0, 0.0, 0.0);
			glVertex3f(p.x, p.y, p.z);

			tmp = actor->getAnimationPlayer()->getGlobalBoneTransform(bone->getParent()->getIndex());
			math::extractTranslation(tmp, p);
			
			glColor3f(1.0, 0.0, 0.0);
			glVertex3f(p.x, p.y, p.z);
		glEnd();
	}
	
	for(SkeletonBone::Iterator i=bone->getBeg(); i!=bone->getEnd(); i++){
		render(scene, actor, *i);
	}

	gl( Enable(GL_DEPTH_TEST) );
	
	gl( LineWidth(currentLineWidth) );
}

void Renderer::setRenderBones(bool r){
	mRenderBones = r;
}

void Renderer::setBoneWidth(float width){
	mBoneWidth = width;
}

void Renderer::setRenderBoundingBoxes(bool state){
	mRenderBoundingBoxes = state;
}

void Renderer::setShaderLightUniforms(Scene* scene, gl::ShaderProgram& prog){
	//// TODO move this to an uniform buffer object so that shaders can share the same 
	//prog.use();

	//DirectionalLight dirLight;
	//scene->getDirectionalLight(dirLight);

	//// directional
	//prog.setUniformVal("uDirectionalLight.ambientItensity",
	//	dirLight.ambientIntensity);

	//prog.setUniformVal("uDirectionalLight.color",
	//	dirLight.color);

	//prog.setUniformVal("uDirectionalLight.diffuseItensity",
	//	dirLight.diffuseIntensity);

	//prog.setUniformVal("uDirectionalLight.direction",
	//	dirLight.direction);

	//// point lights
	//#define setVal(fmt, idx, val) \
	//	do{ \
	//	*bfr=0; \
	//	sprintf(bfr, fmt, idx); \
	//	prog.setUniformVal(bfr, val); \
	//	}while(0)


	//char bfr[100];
	//*bfr=0;

	//uint32_t numActiveLights=0;
	//for(uint32_t i=0; i<scene->getNumPointLights(); i++){
	//	PointLight light;
	//	scene->getPointLight(i, light);

	//	if(light.enabled){
	//		numActiveLights++;
	//	}
	//}


	//prog.setUniformVal("uNumPointLights", (int32_t)numActiveLights);
	//for(uint32_t i=0; i<scene->getNumPointLights(); i++){
	//	PointLight light;
	//	scene->getPointLight(i, light);

	//	if(!light.enabled){
	//		continue;
	//	}

	//	setVal("uPointLights[%d].color", i, light.color);
	//	setVal("uPointLights[%d].ambientItensity", i, light.ambientIntensity);
	//	setVal("uPointLights[%d].diffuseItensity", i, light.diffuseIntensity);
	//	setVal("uPointLights[%d].position", i, light.position);

	//	setVal("uPointLights[%d].attenuation.linear", i, light.attenuation.linear);
	//	setVal("uPointLights[%d].attenuation.constant", i, light.attenuation.constant);
	//	setVal("uPointLights[%d].attenuation.exponential", i, light.attenuation.quadratic);
	//}

	//numActiveLights=0;
	//for(uint32_t i=0; i<scene->getNumSpotLights(); i++){
	//	SpotLight spotLight;
	//	scene->getSpotLight(i, spotLight);

	//	if(spotLight.enabled){
	//		numActiveLights++;
	//	}
	//}
	//prog.setUniformVal("uNumSpotLights", (int32_t)numActiveLights);

	//for(uint32_t i=0; i<scene->getNumSpotLights(); i++){
	//	SpotLight spotLight;
	//	scene->getSpotLight(i, spotLight);

	//	if(!spotLight.enabled){
	//		continue;
	//	}

	//	setVal("uSpotLights[%d].base.color", i, spotLight.color);
	//	setVal("uSpotLights[%d].base.ambientItensity", i, spotLight.ambientIntensity);
	//	setVal("uSpotLights[%d].base.diffuseItensity", i, spotLight.diffuseIntensity);
	//	setVal("uSpotLights[%d].base.position", i, spotLight.position);

	//	setVal("uSpotLights[%d].base.attenuation.linear", i, spotLight.attenuation.linear);
	//	setVal("uSpotLights[%d].base.attenuation.constant", i, spotLight.attenuation.constant);
	//	setVal("uSpotLights[%d].base.attenuation.exponential", i, spotLight.attenuation.quadratic);

	//	setVal("uSpotLights[%d].cutoff", i, cosf(glm::radians(spotLight.cutoff)));
	//	setVal("uSpotLights[%d].direction", i, spotLight.direction);
	//}

	//prog.setUniformVal("uFogParams.density", scene->getFog().density);
	//prog.setUniformVal("uFogParams.color", scene->getFog().color);
	//prog.setUniformVal("uEyePos", scene->getCamera().getPosition());

	//prog.setUniformVal("uMaterial.ambientColor", Color::White());
	//prog.setUniformVal("uMaterial.diffuseColor", Color::White());
	//prog.setUniformVal("uMaterial.specularColor", Color::Black());
	//prog.setUniformVal("uMaterial.shininess", 0.0f);

	//#undef setVal
}

Renderer::RenderState& Renderer::getRenderState(){
	return mRenderState;
}

void Renderer::render(const ATransformable* tf, Camera* camera){
	static const float kLINE_LENGTH = 5.0f;
	static const float kLINE_WIDTH = 3.0f;

	gl( UseProgram(0) );

	// projection
	gl( MatrixMode(GL_PROJECTION) );
	gl( LoadIdentity() );
	gl( LoadMatrixf( glm::value_ptr( camera->getProjectionMatrix() ) ) );

	// moidelview
	gl( MatrixMode(GL_MODELVIEW) );
	gl( LoadIdentity() );
	glm::mat4 mv;
	camera->getCameraMatrix(mv);
	gl( LoadMatrixf( glm::value_ptr(mv) ) );


	glm::vec3 pos, fw, up, right;

	tf->getTranslation(pos);
	tf->getForwardVector(fw);
	tf->getUpVector(up);
	tf->getRightVector(right);

	
	GLint currentLineWidth;
	gl( GetIntegerv(GL_LINE_WIDTH, &currentLineWidth) );

	gl( Disable(GL_BLEND) );
	gl( LineWidth(kLINE_WIDTH) );

	glm::vec3 start = pos;
	glm::vec3 end;

	glColor3f(1.0f, 0.0f, 0.0f);
	{
		glBegin(GL_LINES);

		// X axis - red
		end = pos + right*kLINE_LENGTH;
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(start.x, start.y, start.z);
		glVertex3f(end.x, end.y, end.z);

		// Y axis - green
		end = pos + up*kLINE_LENGTH;
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(start.x, start.y, start.z);
		glVertex3f(end.x, end.y, end.z);

		// Z axis - blue
		end = pos + fw*kLINE_LENGTH;
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(start.x, start.y, start.z);
		glVertex3f(end.x, end.y, end.z);

		glEnd();
	}

	gl( LineWidth(currentLineWidth) );
}

void Renderer::render(const PxBounds3& bounds, Camera* camera, const Color& clr){
	gl( UseProgram(0) );

	// projection
	gl( MatrixMode(GL_PROJECTION) );
	gl( LoadIdentity() );
	gl( LoadMatrixf( glm::value_ptr( camera->getProjectionMatrix() ) ) );

	// moidelview
	gl( MatrixMode(GL_MODELVIEW) );
	gl( LoadIdentity() );
	glm::mat4 mv;
	camera->getCameraMatrix(mv);
	gl( LoadMatrixf( glm::value_ptr(mv) ) );

	gl( Disable(GL_DEPTH_TEST) );
	gl( Disable(GL_BLEND) );

	PxVec3 center = bounds.getCenter();
	PxVec3 extents = bounds.getExtents();

	gl( PolygonMode(GL_FRONT_AND_BACK, GL_LINE) );
	gl( Color4f(clr.red, clr.green, clr.blue, clr.alpha) );

	glBegin(GL_QUADS);
		// bottom
		glVertex3f( center.x - extents.x, center.y - extents.y, center.z - extents.z);
		glVertex3f( center.x - extents.x, center.y - extents.y, center.z + extents.z);
		glVertex3f( center.x + extents.x, center.y - extents.y, center.z + extents.z);
		glVertex3f( center.x + extents.x, center.y - extents.y, center.z - extents.z);

		// top
		glVertex3f( center.x - extents.x, center.y + extents.y, center.z - extents.z);
		glVertex3f( center.x - extents.x, center.y + extents.y, center.z + extents.z);
		glVertex3f( center.x + extents.x, center.y + extents.y, center.z + extents.z);
		glVertex3f( center.x + extents.x, center.y + extents.y, center.z - extents.z);
	glEnd();

	glBegin(GL_LINES);
		glVertex3f( center.x - extents.x, center.y - extents.y, center.z - extents.z);
		glVertex3f( center.x - extents.x, center.y + extents.y, center.z - extents.z);

		glVertex3f( center.x + extents.x, center.y - extents.y, center.z + extents.z);
		glVertex3f( center.x + extents.x, center.y + extents.y, center.z + extents.z);

		glVertex3f( center.x + extents.x, center.y - extents.y, center.z - extents.z);
		glVertex3f( center.x + extents.x, center.y + extents.y, center.z - extents.z);

		glVertex3f( center.x - extents.x, center.y - extents.y, center.z + extents.z);
		glVertex3f( center.x - extents.x, center.y + extents.y, center.z + extents.z);
	glEnd();

	gl( PolygonMode(GL_FRONT_AND_BACK, GL_FILL) );
}

void Renderer::saveScreenshot(const String& path){
	Image img;
	img.fromFrameBuffer(Image::RGB);

	FileIOStream stream(path, AIOStream::eMODE_WRITE);
	DevilImageLoader::getSingleton().save(&stream, &img);
}

void Renderer::setClearColor(const Color& clr){
	mClearColor = clr;

	gl( ClearColor(clr.red, clr.green, clr.blue,
		clr.alpha) );
}

void Renderer::setViewPort(uint32_t width, uint32_t height){
	gl( Viewport(0, 0, width, height) );

	/*
	You may have configured your zNear and zFar clipping planes in a way that severely limits your depth buffer precision.
	Generally, this is caused by a zNear clipping plane value that's too close to 0.0.
	As the zNear clipping plane is set increasingly closer to 0.0, the effective precision of the depth buffer decreases dramatically.
	Moving the zFar clipping plane further away from the eye always has a negative impact on depth buffer precision, but it's not one as dramatic as moving the zNear clipping plane.
	*/
	mViewPort.x = (float)width;
	mViewPort.y = (float)height;

	{
		if(mGodray.quadBatch.isGenerated()){
			initGodray();
		}
	}

	//mFrustum.setPerspectiveProj((float)width, (float)height, 73.0f, 0.5f,
	//	3500.0f);

	mDeferredRenderer->resize(width, height);
}

void Renderer::setPolygoneMode(PolygonMode mode){
	gl( PolygonMode(GL_FRONT_AND_BACK, mode) );
	mRenderState.polygonMode = mode;
}

void Renderer::setFaceCulling(Culling mode){
	gl( Enable(GL_CULL_FACE) );
	gl( CullFace(mode) );
}
	
void Renderer::setRenderAxes(bool state){
	mRenderAxes = state;
}

void Renderer::render(Texture2D* tex, const glm::vec2& viewport, float x, float y, float w, float h, const Color& color, bool flipVertically){
#if 1
	//glClear(GL_COLOR_BUFFER_BIT);
	/*glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);*/
	RectRenderer::getSingleton().draw(viewport, glm::vec2(x, y), glm::vec2(w, h), tex, color);
#else
	gl( UseProgram(0) );

	gl( Viewport(0, 0, viewport.x, viewport.y) );
	// modelviewprojection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, viewport.x, viewport.y, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// bind texture
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex->getTexHandle());

	if(flipVertically){
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glScalef(1.0f, -1.0f, 1.0f);
	}

	// draw textured quad
	glBegin(GL_QUADS);

	glColor4f(clr.red, clr.green, clr.blue, clr.alpha);	

	glVertex2f(x,	y);		glTexCoord2f(1.0, 0.0);
	glVertex2f(x+w, y);		glTexCoord2f(1.0, 1.0);
	glVertex2f(x+w, y+h);	glTexCoord2f(0.0, 1.0);
	glVertex2f(x,	y+h);	glTexCoord2f(0.0, 0.0);

	glEnd();

	if(flipVertically){
		// Revert
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
	}
#endif
}

void Renderer::setShaderMaterialUniforms(Material* material, gl::ShaderProgram& prog){
#ifndef MATERIALS_DISABLED
	prog.setUniformVal("uMaterial.ambientColor", material->getAmbient());
	prog.setUniformVal("uMaterial.diffuseColor", material->getDiffuse());
	prog.setUniformVal("uMaterial.specularColor", material->getSpecular());
	prog.setUniformVal("uMaterial.shininess", material->getShininess());
#else
	prog.setUniformVal("uMaterial.ambientColor", Color::White());
	prog.setUniformVal("uMaterial.diffuseColor", Color::White());
	prog.setUniformVal("uMaterial.specularColor", Color::Black());
	prog.setUniformVal("uMaterial.shininess", 0.0f);
#endif
}


void Renderer::render(Scene& scene, Camera& camera, ARenderer::PassType pass){
	if(!scene.getShadowMappingDesc().enabled && pass == ARenderer::ePASS_SHADOW){
		return;
	}

	// OpenGL state
	mNumRenderedTerrainNodes = 0;
	gl( PolygonMode(GL_FRONT_AND_BACK, mRenderState.polygonMode) );
	

	gl( Enable(GL_DEPTH_TEST) );
	//glDisable(GL_BLEND);

	gl( Viewport(0, 0, mViewPort.x, mViewPort.y) );

	// View matrix
	glm::mat4x4 viewMat;
	camera.getCameraMatrix(viewMat);

	// WARNING: it's very important for the front face to be set as counter-clockwise
	// otherwise this whole concept breaks
	gl( FrontFace(GL_CCW) );

	// Only geometry pass writes to the depth buffer
	gl( DepthMask(true) );
	gl( Enable(GL_DEPTH_TEST) );

	if(pass == ARenderer::ePASS_NORMAL){
		mDeferredRenderer->useDepthTexture(DeferredRender::eDEPTH_TEXTURE_NORMAL);
	}
	else if(pass == ARenderer::ePASS_SHADOW){
		mDeferredRenderer->useDepthTexture(DeferredRender::eDEPTH_TEXTURE_SHADOW_MAP);
	}
	
	// Setup the renderer
	mDeferredRenderer->startFrame();

	// Bind the drawing buffers (3 textures)
	mDeferredRenderer->bindForGeometryPass();

	// Clear them all, along with the depth/stencil buffer
	gl( ClearColor(mClearColor.red, mClearColor.green, mClearColor.blue, 0) );
	gl( Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );

	// Render all the geometry to the G buffer
	for(RendererList::iterator iter=mSceneRenderers.begin(); iter!=mSceneRenderers.end(); iter++){
		if((*iter)->isDeferred()){
			//
			// TODO: DISABLE COLOR WRITES FOR EACH RENDERER FOR SHADOW PASS !
			//  glDrawBuffer(GL_NONE); // No color buffer is drawn to.
			//
			(*iter)->render(&scene, &camera, pass,
				pass == ARenderer::ePASS_NORMAL ? mDeferredRenderer->getDepthTexture(DeferredRender::eDEPTH_TEXTURE_SHADOW_MAP) : NULL);
		}
	}
	
	if(pass == ARenderer::ePASS_SHADOW){
		return;
	}

	// Do the light rendering (e.g. directional, point and spot lights)
	mDeferredRenderer->doLightPass(&scene, &scene.getCamera());

#if 0
	if(pass == ARenderer::ePASS_NORMAL){
		Texture2D::depthDump(mDeferredRenderer->getDepthTexture(DeferredRender::eDEPTH_TEXTURE_SHADOW_MAP), "depth.bmp");
	}
#endif

	const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT4 };
	gl( DrawBuffers(1, drawBuffers) );
	gl( Disable(GL_BLEND) );
	gl( Enable(GL_DEPTH_TEST) );
	
	// Render the non-deferred geometry
	for(RendererList::iterator iter=mSceneRenderers.begin(); iter!=mSceneRenderers.end(); iter++){
		if(!(*iter)->isDeferred()){
			(*iter)->render(&scene, &camera, pass, NULL);
		}
	}

	// Final pass (render the resulting texture to the screen)
	mDeferredRenderer->bindForFinalPass();
	mDeferredRenderer->getFrameBuffer()->blit(
		glm::vec4(0, 0, mViewPort.x, mViewPort.y),
		glm::vec4(0, 0, mViewPort.x, mViewPort.y),
			GL_COLOR_ATTACHMENT4, GL_COLOR_BUFFER_BIT, GL_LINEAR
	);

	// Enable for debug blits
#if 0
	GLint doubleBuffered; 
	gl( GetIntegerv(GL_DOUBLEBUFFER, &doubleBuffered) );

	GLenum defaultBfrs[1];
	defaultBfrs[0] = doubleBuffered? GL_BACK : GL_FRONT;

	gl( DrawBuffers(1, defaultBfrs) );

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mDeferredRenderer->debugBlit(glm::vec2(1280, 720));
#endif

	gl::FrameBuffer::unbind();
}

void Renderer::godrayPass(Scene& scene){
	// TODO this code should be moved elsewhere (a separate GodrayRenderer class perhaps?)

	Scene::GodRayParams godrayParams;
	scene.getGodRayParams(godrayParams);

	if(!godrayParams.enabled){
		return;
	}

	// Sceen view matrix
	glm::mat4 viewMatrix;
	scene.getCamera().getCameraMatrix(viewMatrix);

	viewMatrix[3][0] = 0.0f;
	viewMatrix[3][1] = 0.0f;
	viewMatrix[3][2] = 0.0f;

	// Rendering viewport
	glm::vec4 viewport(0, 0, mViewPort.x, mViewPort.y);

	// Position of the source on the screen
	glm::vec3 sourceScreenPos = glm::project(godrayParams.sourcePosition, viewMatrix, scene.getCamera().getProjectionMatrix(), viewport);

	// Is the source visible ?
	bool sunVisible = sourceScreenPos.x >= 0.0f && sourceScreenPos.y >= 0.0f && sourceScreenPos.x <= mViewPort.x && sourceScreenPos.y <= mViewPort.y;

	// Pass 1
	{
		// Setup FBO
		mGodray.frameBuffer.bind(gl::FrameBuffer::eMODE_DRAW);
		const GLenum pass1Buffers[] = {GL_COLOR_ATTACHMENT0};
		gl( DrawBuffers(1, pass1Buffers) );

		// Draw background
		setClearColor(godrayParams.rayColor);
		gl( Clear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT) );

		// Setup source GL params
		gl( Enable(GL_BLEND) );
		gl( BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );
		gl( BlendEquation(GL_FUNC_ADD) );

		gl( Enable(GL_POINT_SPRITE) );
		gl( Enable(GL_PROGRAM_POINT_SIZE) );

		// Setup source shader
		mGodray.sourceShader.use();

		mGodray.sourceShader.setUniformVal("uModelMat", glm::translate(godrayParams.sourcePosition));
		mGodray.sourceShader.setUniformVal("uViewMat", viewMatrix);
		mGodray.sourceShader.setUniformVal("uProjMat", scene.getCamera().getProjectionMatrix());
		mGodray.sourceShader.setUniformVal("uSunSize", godrayParams.sourceSize);
		mGodray.sourceShader.setUniformVal("uPlanetTexture", 0);
		mGodray.sourceShader.setUniformVal("uSourceColor", godrayParams.sourceColor);

		gl( ActiveTexture(GL_TEXTURE0) );

		if(godrayParams.sourceTexture){
			godrayParams.sourceTexture->bind();
		}
		else{
			// Use default source texture if none is provided
			mGodray.defaultSourceTexture.bind();
		}

		gl( Color4f(godrayParams.sourceColor.red, 
			godrayParams.sourceColor.green,
			godrayParams.sourceColor.blue,
			godrayParams.sourceColor.alpha
		));

		// Render the source
		mGodray.sourceBatch.render();

		// Use the godray texture from the deferred renderer to render entire
		// scene on top of the source/background previously rendered

		gl( Enable(GL_BLEND) );
		gl( BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );
		gl( BlendEquation(GL_FUNC_ADD) );

		render(mDeferredRenderer->getGTexture(DeferredRender::eGTEX_GODRAY), mViewPort, 0, 0, mViewPort.x, mViewPort.y, Color::White(), true);

	#if 0
		// Debug dump of the first pass
		mGodrayPass1.dump("godray_pass1.bmp");
	#endif
	}

	// We now have a rendered background (solid color), a rendered source (provided texture) 
	// and the entire scene rendered in single color on top of it, so we are ready to do
	// the actual post-processing effect

	const GLenum pass2Buffers[] = {GL_COLOR_ATTACHMENT1};
	gl( DrawBuffers(1, pass2Buffers) );

	gl( ActiveTexture(GL_TEXTURE0) );
	mGodray.pass1.bind();

	// Setup post-processing shader
	mGodray.shader.use();
	mGodray.shader.setUniformVal("uExposure", godrayParams.exposure);
	mGodray.shader.setUniformVal("uDecay", godrayParams.decay);
	mGodray.shader.setUniformVal("uDensity", godrayParams.density);
	mGodray.shader.setUniformVal("uWeight", godrayParams.weight);
	mGodray.shader.setUniformVal("uNumSamples", (int)godrayParams.sampleNumber);
	mGodray.shader.setUniformVal("uRectImage", 0);
	mGodray.shader.setUniformVal("uLightPositionOnScreen", glm::vec2(sourceScreenPos.x, sourceScreenPos.y));
	mGodray.shader.setUniformVal("uMVPMat", glm::ortho(
		0.0f, mViewPort.x, 0.0f, mViewPort.y));

	gl( Disable(GL_BLEND) );
	gl( Disable(GL_DEPTH_TEST) );
	gl( Disable(GL_CULL_FACE) );

	// Do the post-processing effect
	mGodray.quadBatch.render();

	// We now have a texture with a complete godray effect
	// so we just have to render it on top of the normal scene


	// Use default framebuffer
	// TODO account for the RenderTarget provided in main render call
	gl::FrameBuffer::unbind(gl::FrameBuffer::eMODE_DRAW);

	GLint doubleBuffered; 
	gl( GetIntegerv(GL_DOUBLEBUFFER, &doubleBuffered) );

	GLenum defaultBfrs[1];
	defaultBfrs[0] = doubleBuffered? GL_BACK : GL_FRONT;

	gl( DrawBuffers(1, defaultBfrs) );

	mRectShader.use();

	gl( ActiveTexture(GL_TEXTURE0) );

	mRectShader.setUniformVal("uRectImage", 0);
	mRectShader.setUniformVal("uMVPMat", glm::ortho(
		0.0f, mViewPort.x, 0.0f, mViewPort.y));

	gl( Enable(GL_BLEND) );
	gl( BlendFunc(GL_ONE, GL_ONE) );
	gl( Disable(GL_DEPTH_TEST) );

	mGodray.quadBatch.render();
}

void Renderer::render(Scene& scene, gui::Window* window){
	// Draw all the elements
	window->redraw();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	{
		// TODO window draw changes framebuffers, do something about that
		// Use default framebuffer
		gl::FrameBuffer::unbind(gl::FrameBuffer::eMODE_DRAW);

		GLint doubleBuffered; 
		gl( GetIntegerv(GL_DOUBLEBUFFER, &doubleBuffered) );

		GLenum defaultBfrs[1];
		defaultBfrs[0] = doubleBuffered? GL_BACK : GL_FRONT;

		gl( DrawBuffers(1, defaultBfrs) );

		gl( Viewport(0, 0, mViewPort.x, mViewPort.y) );
	}

	// Render the GUI texture on top of everything
	render(
		window->getCanvas().getTexture(),
		glm::vec2(mViewPort.x, mViewPort.y),
		window->getRect().x, window->getRect().y,
		window->getRect().width, window->getRect().height,
		window->getBackgroundColor()
	);
}

void Renderer::render(Scene& scene, RenderTarget* target){
	setClearColor(Color::Magenta());
	
	if(!target){
		// Use default framebuffer
		gl::FrameBuffer::unbind(gl::FrameBuffer::eMODE_DRAW);

		GLint doubleBuffered; 
		gl( GetIntegerv(GL_DOUBLEBUFFER, &doubleBuffered) );

		GLenum defaultBfrs[1];
		defaultBfrs[0] = doubleBuffered? GL_BACK : GL_FRONT;

		gl( DrawBuffers(1, defaultBfrs) );
	}
	else{
		target->bind();
	}

	gl( Clear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT) );
	gl( Enable(GL_CULL_FACE) );
	gl( Disable(GL_BLEND) );

	// Render entire scene
	const Scene::ShadowMappingDesc& shadowMapping = scene.getShadowMappingDesc();
	if(shadowMapping.enabled){
		render(scene, const_cast<Camera&>(shadowMapping.casterSource), ARenderer::ePASS_SHADOW);	
	}

	// TODO this seriously needs to be optimized (calling this function with an empty scene cuts down the framerate
	// from ~1500 to ~200
	render(scene, scene.getCamera(), ARenderer::ePASS_NORMAL);
	
	// Do the godray post-processing effect if enabled
	godrayPass(scene);

	// Render UI
	if(scene.getWindowManager()){
		for(gui::WindowManager::WindowSet::iterator iter=scene.getWindowManager()->getWindowsBeg(); iter!=scene.getWindowManager()->getWindowsEnd(); iter++){
			render(scene, *iter);
		}
	}

	if(!target){
		// Use default framebuffer
		gl::FrameBuffer::unbind(gl::FrameBuffer::eMODE_DRAW);

		GLint doubleBuffered; 
		gl( GetIntegerv(GL_DOUBLEBUFFER, &doubleBuffered) );

		GLenum defaultBfrs[1];
		defaultBfrs[0] = doubleBuffered? GL_BACK : GL_FRONT;

		gl( DrawBuffers(1, defaultBfrs) );
	}
	else{
		target->bind();
	}

	// Debug info rendering
	{
		// Skeleton bones
		if(mRenderBones){
			for(Scene::ActorSet::const_iterator iter=scene.getActorSet(ASceneActor::eTYPE_MODELLED).begin(); iter!=scene.getActorSet(ASceneActor::eTYPE_MODELLED).end(); iter++){
				const ModelledActor* actor = dynamic_cast<const ModelledActor*>(*iter);

				if(actor->getModel() && actor->getModel()->getSkeleton()){
					render(&scene, actor, actor->getModel()->getSkeleton());
				}
			}
		}

		// Bounding boxes
		if(mRenderBoundingBoxes){
			glDisable(GL_DEPTH_TEST);
			for(Scene::ActorMap::iterator i=scene.getActorMap().begin(); i!=scene.getActorMap().end(); i++){
				render(i->second->getBounds(), &scene.getCamera(), i->second->getBoundingBoxColor());
			}
			glEnable(GL_DEPTH_TEST);
		}

		// Axes
		if(mRenderAxes){
			glDisable(GL_DEPTH_TEST);
			for(Scene::ActorMap::iterator i=scene.getActorMap().begin(); i!=scene.getActorMap().end(); i++){
				render(i->second->getTransformable(), &scene.getCamera());
			}
			glEnable(GL_DEPTH_TEST);
		}
	}
}

void GodRayShader::create(){
	ShaderFactory::createShader(*this, "godray.vp", "godray.fp");

	bindAttribLocation(0, "inPosition");
	bindAttribLocation(1, "inTexCoord");

	link();
}

void RectShader::create(){
	ShaderFactory::createShader(*this, "rect.vp", "rect.fp");

	bindAttribLocation(0, "inPosition");
	bindAttribLocation(1, "inTexCoord");

	link();
}

}; // </wt