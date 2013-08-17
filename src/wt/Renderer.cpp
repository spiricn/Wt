#include "wt/stdafx.h"

#include "wt/Renderer.h"
#include "wt/DevilImageLoader.h"
#include "wt/RenderBuffer.h"
#include "wt/Singleton.h"


#include "wt/ParticleRenderer.h"
#include "wt/ModelRenderer.h"
#include "wt/TerrainRenderer.h"
#include "wt/DeferredRenderer.h"

#define TD_TRACE_TAG "Renderer"



namespace wt{


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

Renderer::Renderer(EventManager* eventManager) : mClearColor(0.5, 0, 0, 1.0f),
	mGodrayPass1(NULL, 0, "", Texture2D::eRECT_TEXTURE), mGodrayPass2(NULL, 0, "", Texture2D::eRECT_TEXTURE),
	mRenderBones(false), mBoneWidth(3.0f), mRenderBoundingBoxes(false), mEventManager(eventManager), mDeferredRenderer(NULL){

	mEventManager->registerListener(this, SceneLightingModifiedEvent::TYPE);
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

	setViewPort(portW, portH);

	 //Attach the renderers (order matters)
	attachRenderer(new TerrainRenderer);
	attachRenderer(new ModelRenderer);
	attachRenderer(new SkyboxRenderer);
	attachRenderer(new ParticleRenderer);

	LOGV("Compiling godray shader ...");
	mGodRayShader.create();

	LOGV("Compiling rect shader ...");
	mRectShader.create();
	

	mGodraySunShader.createFromFiles("shaders/godraysun.vp", "shaders/godraysun.fp");
	mGodraySunShader.bindAttribLocation(0, "inPosition");
	mGodraySunShader.link();



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
		mSunBatch.create(
			GL_POINTS,
			vertices, 1, sizeof(float)*3,
			indices, 1, sizeof(uint32_t)
		);

		mSunBatch.setVertexAttribute(0, 3, GL_FLOAT, 0);
	}
	setClearColor(Color(0.3, 0.3, 0.3));


	// TODO acquire this from elsewhere
	FileIOStream stream("d:\\Documents\\prog\\c++\\workspace\\Wt\\rsrc\\godray_sun.png", AIOStream::eMODE_READ);
	TextureLoader::getSingleton().load(&stream, &mGodraySunTexture);



	LOGV("Initialized OK");
}

Renderer::~Renderer(){
	for(RendererList::iterator iter=mSceneRenderers.begin(); iter!=mSceneRenderers.end(); iter++){
		delete *iter;
	}

	mSceneRenderers.clear();
}

void Renderer::attachRenderer(ARenderer* renderer){
	renderer->create();
	mSceneRenderers.push_back(renderer);
}

bool Renderer::handleEvent(const Sp<Event> evt){
	if(evt->getType() == SceneLightingModifiedEvent::TYPE){

		LOGV("Scene lighting changed");

		Scene* scene = ((SceneLightingModifiedEvent*)evt.get())->scene;

		setShaderLightUniforms(scene, *mDeferredRenderer->getLightShader(DeferredRender::eLIGHT_SHADER_DIRECTIONAL) );
		setShaderLightUniforms(scene, *mDeferredRenderer->getLightShader(DeferredRender::eLIGHT_SHADER_POINT) );

		for(RendererList::iterator iter=mSceneRenderers.begin(); iter!=mSceneRenderers.end(); iter++){
			(*iter)->onSceneLightingChanged(scene, this);

			/*gl::ShaderProgram* rendererProg = (*iter)->setupSceneLighting();

			if(rendererProg != NULL){
				setShaderLightUniforms(scene, *rendererProg);
			}*/
		}
	}

	return false;
}
void Renderer::initGodray(){
	{ // Godray setup
		
		if(mGodrayFBO.isCreated()){
			mGodrayFBO.destroy();

			mGodrayDepthBuffer.destroy();
		}

		// Depth buffer
		mGodrayDepthBuffer.create();
		mGodrayDepthBuffer.setStorage(GL_DEPTH_COMPONENT, mViewPort.x, mViewPort.y);

		mGodrayFBO.create();

		mGodrayFBO.bind(gl::FrameBuffer::eMODE_DRAW);

		mGodrayPass1.create();
		mGodrayPass1.bind();
		gl( PixelStorei(GL_UNPACK_ALIGNMENT, 1) );

		float portW = mViewPort.x;
		float portH = mViewPort.y;
		mGodrayPass1.setData(portW, portH,
			GL_RGBA, GL_RGBA, NULL, GL_UNSIGNED_BYTE, false);

		mGodrayPass2.create();
		mGodrayPass2.bind();
		gl( PixelStorei(GL_UNPACK_ALIGNMENT, 1) );
		mGodrayPass2.setData(portW, portH,
			GL_RGBA, GL_RGBA, NULL, GL_UNSIGNED_BYTE, false);

		mGodrayFBO.addAttachment(GL_COLOR_ATTACHMENT0, &mGodrayPass1);
		mGodrayFBO.addAttachment(GL_COLOR_ATTACHMENT1, &mGodrayPass2);
		mGodrayFBO.addAttachment(GL_DEPTH_ATTACHMENT, mGodrayDepthBuffer);

		WT_ASSERT(mGodrayFBO.isComplete(), "Godray FBO incomplete");

		struct vertex{
			float x,y;
			float s,t;
		};

		vertex vertices[] = {
			{0, 0, 0, 0}, {portW, 0, portW, 0}, {portW, portH, portW, portH}, {0, portH, 0, portH}
		};

		uint32_t indices[4] = {0, 1, 2, 3};

		mGodrayBatch.destroy();
		mGodrayBatch.create(
			GL_QUADS,
			vertices, 4, sizeof(vertex),
			indices, 4, sizeof(uint32_t)
		);

		// inPosition
		mGodrayBatch.setVertexAttribute(0, 2, GL_FLOAT, offsetof(vertex, x));
		// inTexCoord
		mGodrayBatch.setVertexAttribute(1, 2, GL_FLOAT, offsetof(vertex, s));
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
	gl( LoadMatrixf(glm::value_ptr(mFrustum.getProjMatrix())) );

	glm::mat4 tmp;
	glm::vec3 p;

	
	// Setup modelview matrix
	glm::mat4 view;
	glm::mat4 model;
	actor->getTransform().getMatrix(model);
	scene->getCamera().getMatrix(view);
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
		gl( End() );
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
	
void Renderer::setFOV(float angle){
	mFrustum.setPerspectiveProj(
		mViewPort.x, mViewPort.y, angle, mFrustum.getNearClip(), mFrustum.getFarClip()
	);
}

void Renderer::setShaderLightUniforms(Scene* scene, gl::ShaderProgram& prog){
	// TODO move this to an uniform buffer object so that shaders can share the same 
	prog.use();

	DirectionalLight dirLight;
	scene->getDirectionalLight(dirLight);

	// directional
	prog.setUniformVal("uDirectionalLight.ambientItensity",
		dirLight.mAmbientIntesity);

	prog.setUniformVal("uDirectionalLight.color",
		dirLight.mColor);

	prog.setUniformVal("uDirectionalLight.diffuseItensity",
		dirLight.mDiffuseItensity);

	prog.setUniformVal("uDirectionalLight.direction",
		dirLight.mDirection);

	// point lights
	#define setVal(fmt, idx, val) \
		do{ \
		*bfr=0; \
		sprintf(bfr, fmt, idx); \
		prog.setUniformVal(bfr, val); \
		}while(0)


	char bfr[100];
	*bfr=0;

	uint32_t numActiveLights=0;
	for(uint32_t i=0; i<scene->getNumPointLights(); i++){
		PointLight light;
		scene->getPointLight(i, light);

		if(light.mActive){
			numActiveLights++;
		}
	}


	prog.setUniformVal("uNumPointLights", (int32_t)numActiveLights);
	for(uint32_t i=0; i<scene->getNumPointLights(); i++){
		PointLight light;
		scene->getPointLight(i, light);

		if(!light.mActive){
			continue;
		}

		setVal("uPointLights[%d].color", i, light.mColor);
		setVal("uPointLights[%d].ambientItensity", i, light.mAmbientIntesity);
		setVal("uPointLights[%d].diffuseItensity", i, light.mDiffuseItensity);
		setVal("uPointLights[%d].position", i, light.mPosition);

		setVal("uPointLights[%d].attenuation.linear", i, light.mAttenuation.linear);
		setVal("uPointLights[%d].attenuation.constant", i, light.mAttenuation.constant);
		setVal("uPointLights[%d].attenuation.exponential", i, light.mAttenuation.exponential);
	}

	numActiveLights=0;
	for(uint32_t i=0; i<scene->getNumSpotLights(); i++){
		SpotLight spotLight;
		scene->getSpotLight(i, spotLight);

		if(spotLight.mActive){
			numActiveLights++;
		}
	}
	prog.setUniformVal("uNumSpotLights", (int32_t)numActiveLights);

	for(uint32_t i=0; i<scene->getNumSpotLights(); i++){
		SpotLight spotLight;
		scene->getSpotLight(i, spotLight);

		if(!spotLight.mActive){
			continue;
		}

		setVal("uSpotLights[%d].base.color", i, spotLight.mColor);
		setVal("uSpotLights[%d].base.ambientItensity", i, spotLight.mAmbientIntesity);
		setVal("uSpotLights[%d].base.diffuseItensity", i, spotLight.mDiffuseItensity);
		setVal("uSpotLights[%d].base.position", i, spotLight.mPosition);

		setVal("uSpotLights[%d].base.attenuation.linear", i, spotLight.mAttenuation.linear);
		setVal("uSpotLights[%d].base.attenuation.constant", i, spotLight.mAttenuation.constant);
		setVal("uSpotLights[%d].base.attenuation.exponential", i, spotLight.mAttenuation.exponential);

		setVal("uSpotLights[%d].cutoff", i, cosf(glm::radians(spotLight.cutoff)));
		setVal("uSpotLights[%d].direction", i, spotLight.direction);
	}

	prog.setUniformVal("uFogParams.density", scene->getFog().density);
	prog.setUniformVal("uFogParams.color", scene->getFog().color);
	prog.setUniformVal("uEyePos", scene->getCamera().getPosition());

	prog.setUniformVal("uMaterial.ambientColor", Color::white());
	prog.setUniformVal("uMaterial.diffuseColor", Color::white());
	prog.setUniformVal("uMaterial.specularColor", Color::black());
	prog.setUniformVal("uMaterial.shininess", 0.0f);

	#undef setVal
}

Renderer::RenderState& Renderer::getRenderState(){
	return mRenderState;
}

void Renderer::render(const PxBounds3& bounds, math::Camera* camera, const Color& clr){
	gl( UseProgram(0) );

	// projection
	gl( MatrixMode(GL_PROJECTION) );
	gl( LoadIdentity() );
	gl( LoadMatrixf( glm::value_ptr( mFrustum.getProjMatrix() ) ) );

	// moidelview
	gl( MatrixMode(GL_MODELVIEW) );
	gl( LoadIdentity() );
	glm::mat4 mv;
	camera->getMatrix(mv);
	gl( LoadMatrixf( glm::value_ptr(mv) ) );

	PxVec3 center = bounds.getCenter();
	PxVec3 extents = bounds.getExtents();

	gl( PolygonMode(GL_FRONT_AND_BACK, GL_LINE) );
	gl( Color4f(clr.mRed, clr.mGreen, clr.mBlue, clr.mAlpha) );

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

const math::Frustum& Renderer::getFrustum() const{
	return mFrustum;
}

void Renderer::setClearColor(const Color& clr){
	mClearColor = clr;

	gl( ClearColor(clr.mRed, clr.mGreen, clr.mBlue,
		clr.mAlpha) );
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
		if(mGodrayBatch.isGenerated()){
			initGodray();
		}
	}

	mFrustum.setPerspectiveProj((float)width, (float)height, 73.0f, 0.5f,
		3500.0f);

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


void Renderer::render(Texture2D* tex, const glm::vec2& viewport, float x, float y, float w, float h, const Color& clr){
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
	glBindTexture(GL_TEXTURE_2D, tex->getTexHandle());

	// draw textured quad
	glBegin(GL_QUADS);

	glColor4f(clr.mRed, clr.mGreen, clr.mBlue, clr.mAlpha);

	glVertex2f(x,	y);		glTexCoord2f(1.0, 0.0);
	glVertex2f(x+w, y);		glTexCoord2f(1.0, 1.0);
	glVertex2f(x+w, y+h);	glTexCoord2f(0.0, 1.0);
	glVertex2f(x,	y+h);	glTexCoord2f(0.0, 0.0);

	glEnd();
}

void Renderer::setShaderMaterialUniforms(Material* material, gl::ShaderProgram& prog){
#ifndef MATERIALS_DISABLED
	prog.setUniformVal("uMaterial.ambientColor", material->getAmbient());
	prog.setUniformVal("uMaterial.diffuseColor", material->getDiffuse());
	prog.setUniformVal("uMaterial.specularColor", material->getSpecular());
	prog.setUniformVal("uMaterial.shininess", material->getShininess());
#else
	prog.setUniformVal("uMaterial.ambientColor", Color::white());
	prog.setUniformVal("uMaterial.diffuseColor", Color::white());
	prog.setUniformVal("uMaterial.specularColor", Color::black());
	prog.setUniformVal("uMaterial.shininess", 0.0f);
#endif
}


void Renderer::render(Scene& scene, ARenderer::PassType pass){
	// OpenGL state
	mNumRenderedTerrainNodes = 0;
	gl( PolygonMode(GL_FRONT_AND_BACK, mRenderState.polygonMode) );

	gl( Enable(GL_DEPTH_TEST) );
	//glDisable(GL_BLEND);

	gl( Viewport(0, 0, mViewPort.x, mViewPort.y) );

	// View matrix
	glm::mat4x4 viewMat;
	scene.getCamera().getMatrix(viewMat);

	// WARNING: it's very important for the front face to be set as counter-clockwise
	// otherwise this whole concept breaks
	glFrontFace(GL_CCW);

	// Only geometry pass writes to the depth buffer
	gl( DepthMask(true) );
	gl( Enable(GL_DEPTH_TEST) );

	// Setup the renderer
	mDeferredRenderer->startFrame();

	// Bind the drawing buffers (3 textures)
	mDeferredRenderer->bindForGeometryPass();

	// Clear them all, along with the depth/stencil buffer
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render all the geometry to the G buffer
	for(RendererList::iterator iter=mSceneRenderers.begin(); iter!=mSceneRenderers.end(); iter++){
		if((*iter)->isDeferred()){
			(*iter)->render(&scene, &scene.getCamera(), ARenderer::ePASS_NORMAL);
		}
	}


	// When we get here the depth buffer is already populated and the stencil pass
    // depends on it, but it does not write to it.
    gl( DepthMask(GL_FALSE) );
	gl( Enable(GL_STENCIL_TEST ));

	for(uint32_t i=0; i<scene.getNumPointLights(); i++){
		PointLight light;

		scene.getPointLight(i, light);
		if(!light.mActive){
			continue;
		}

		mDeferredRenderer->stencilPass(&scene, &scene.getCamera(), i);
		mDeferredRenderer->pointLightPass(&scene, &scene.getCamera(), i);
	}

	// The directional light does not need a stencil test because its volume
    // is unlimited and the final pass simply copies the texture.

	gl( Disable(GL_STENCIL_TEST ));

	mDeferredRenderer->directionalLightPass(&scene, &scene.getCamera());


	const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT3 };
	gl( DrawBuffers(1, drawBuffers) );
	gl( Disable(GL_BLEND) );
	gl( Enable(GL_DEPTH_TEST) );

	// Render the non-deferred geometry
	for(RendererList::iterator iter=mSceneRenderers.begin(); iter!=mSceneRenderers.end(); iter++){
		if(!(*iter)->isDeferred()){
			(*iter)->render(&scene, &scene.getCamera(), ARenderer::ePASS_NORMAL);
		}
	}

	// Final pass (render the resulting texture to the screen)
	mDeferredRenderer->bindForFinalPass();
	mDeferredRenderer->getFrameBuffer()->blit(
		glm::vec4(0, 0, mViewPort.x, mViewPort.y),
		glm::vec4(0, 0, mViewPort.x, mViewPort.y),
			GL_COLOR_ATTACHMENT3, GL_COLOR_BUFFER_BIT, GL_LINEAR
	);
	
#if 0
	//// Debug blit
	GLint doubleBuffered; 
	gl( GetIntegerv(GL_DOUBLEBUFFER, &doubleBuffered) );

	GLenum defaultBfrs[1];
	defaultBfrs[0] = doubleBuffered? GL_BACK : GL_FRONT;

	gl( DrawBuffers(1, defaultBfrs) );

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mDeferredRenderer->debugBlit(glm::vec2(1280, 720));
#endif

#if 0
	if(pass == ARenderer::ePASS_NORMAL){
		// Skeleton bones
		if(mRenderBones){
			for(Scene::ModelledActorSet::const_iterator iter=scene.getModelledActors().cbegin(); iter!=scene.getModelledActors().cend(); iter++){
				if((*iter)->getModel() && (*iter)->getModel()->getSkeleton()){
					render(&scene, (*iter), (*iter)->getModel()->getSkeleton());
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
	}
#endif

	gl::FrameBuffer::unbind();
}

void Renderer::render(Scene& scene, RenderTarget* target){
	setClearColor(mClearColor);

	glm::mat4 modelview;
	scene.getCamera().getMatrix(modelview, true);



	Scene::GodRayParams godrayParams;
	scene.getGodRayParams(godrayParams);
	
	glm::vec4 viewport(0, 0, mViewPort.x, mViewPort.y);
	glm::vec3 sunScreenPos = glm::project(godrayParams.sourcePosition, modelview, mFrustum.getProjMatrix(), viewport);
	bool sunVisible = sunScreenPos.x >= 0.0f && sunScreenPos.y >= 0.0f && sunScreenPos.x <= mViewPort.x && sunScreenPos.y <= mViewPort.y;

	if(godrayParams.enabled){ 
		// pass 1 (render scene without textures/lighting)
		mGodrayFBO.bind(gl::FrameBuffer::eMODE_DRAW);

		const GLenum pass1Buffers[] = {GL_COLOR_ATTACHMENT0};
		gl( DrawBuffers(1, pass1Buffers) );

		setClearColor(godrayParams.rayColor);
		gl( Clear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT) );

		// draw sun
		gl( Enable(GL_BLEND) );
		gl( BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );
		gl( BlendEquation(GL_FUNC_ADD) );
		//gl( BlendFunc(GL_SRC_ALPHA, GL_ONE) );

		gl( Enable(GL_POINT_SPRITE) );
		gl( Enable(GL_PROGRAM_POINT_SIZE) );

		mGodraySunShader.use();
		glColor3f(1, 0, 0);
		//glm::mat4 sunViewMat;
		//scene.getCamera().getMatrix(sunViewMat, 1);
		mGodraySunShader.setUniformVal("uModelMat", glm::translate(godrayParams.sourcePosition));
		mGodraySunShader.setUniformVal("uViewMat", modelview);
		mGodraySunShader.setUniformVal("uProjMat", mFrustum.getProjMatrix());
		mGodraySunShader.setUniformVal("uSunSize", godrayParams.sourceSize);
		mGodraySunShader.setUniformVal("uPlanetTexture", 0);
		mGodraySunShader.setUniformVal("uSourceColor", godrayParams.sourceColor);


		gl( ActiveTexture(GL_TEXTURE0) );
		if(godrayParams.sourceTexture){
			godrayParams.sourceTexture->bind();
		}
		else{
			mGodraySunTexture.bind();
		}

		gl( Color4f(godrayParams.sourceColor.mRed, 
			godrayParams.sourceColor.mGreen,
			godrayParams.sourceColor.mBlue,
			godrayParams.sourceColor.mAlpha
			) );

		mSunBatch.render();

		gl( Disable(GL_BLEND) );

		// render the entire scene (without textures and lighting)
		gl( Enable(GL_CULL_FACE) );
		render(scene, ARenderer::ePASS_GODRAY);


		//mGodrayPass1.dump("pass1.bmp");
	}

	if(godrayParams.enabled){ 
		// pass 2 (do the 2D post processing effect)

		const GLenum pass2Buffers[] = {GL_COLOR_ATTACHMENT1};
		gl( DrawBuffers(1, pass2Buffers) );

		mGodRayShader.use();

		// pass 1 result texture
		gl( ActiveTexture(GL_TEXTURE0) );
		mGodrayPass1.bind();

		mGodRayShader.use();
		mGodRayShader.setUniformVal("uExposure", godrayParams.exposure);
		mGodRayShader.setUniformVal("uDecay", godrayParams.decay);
		mGodRayShader.setUniformVal("uDensity", godrayParams.density);
		mGodRayShader.setUniformVal("uWeight", godrayParams.weight);
		mGodRayShader.setUniformVal("uNumSamples", (int)godrayParams.sampleNumber);

		mGodRayShader.setUniformVal("uRectImage", 0);
		mGodRayShader.setUniformVal("uLightPositionOnScreen", glm::vec2(sunScreenPos.x, sunScreenPos.y));


		mGodRayShader.setUniformVal("uMVPMat", glm::ortho(
			0.0f, mViewPort.x, 0.0f, mViewPort.y));

		gl( Disable(GL_BLEND) );
		gl( Disable(GL_DEPTH_TEST) );
		gl( Disable(GL_CULL_FACE) );
		mGodrayBatch.render();


		//mGodrayPass2.dump("pass2.bmp");
	}
	

	{ 
		// pass 3
		if(!target){
			// Default framebuffer
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
		render(scene, ARenderer::ePASS_NORMAL);
	}

	if(godrayParams.enabled){
		// pass 4 (apply post processing effect)

		mRectShader.use();

		gl( ActiveTexture(GL_TEXTURE0) );
		mGodrayPass2.bind();

		mRectShader.setUniformVal("uRectImage", 0);
		mRectShader.setUniformVal("uMVPMat", glm::ortho(
			0.0f, mViewPort.x, 0.0f, mViewPort.y));

		gl( Enable(GL_BLEND) );
		gl( BlendFunc(GL_ONE, GL_ONE) );
		gl( Disable(GL_DEPTH_TEST) );
		

		mGodrayBatch.render();
	}
	/*glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	float s=100.0f;
	float x = sunScreenPos.x - s/2;
	float y = (mViewPort.y - sunScreenPos.y) - s/2;

	render(TextureManager::getSingleton().getFromPath("$ROOT/brushes/circle_hard"), mViewPort,
		x < 0 ? 0 : x+s > mViewPort.x ? mViewPort.x-s : x,
		y < 0 ? 0 : y+s > mViewPort.y ? mViewPort.y-s : y,
		s, s, Color::green());*/
}

}; // </wt