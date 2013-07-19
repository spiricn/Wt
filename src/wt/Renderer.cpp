#include "wt/stdafx.h"

#include "wt/Renderer.h"
#include "wt/DevilImageLoader.h"
#include "wt/RenderBuffer.h"
#include "wt/Singleton.h"

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

Renderer::Renderer() : mClearColor(0.5, 0, 0, 1.0f),
	mGodrayPass1(0, "", Texture2D::eRECT_TEXTURE), mGodrayPass2(0, "", Texture2D::eRECT_TEXTURE),
	mRenderBones(false), mBoneWidth(3.0f), mRenderBoundingBoxes(false){
}

void Renderer::init(uint32_t portW, uint32_t portH ){
	WT_ASSERT(portW > 0 && portH > 0, "Invalid viewport size (%d x %d)", portW, portH);

	LOGV("Initializing...");

	LOGV("GL_VERSION = %s", glGetString(GL_VERSION));
	LOGV("GL_VENDOR = %s", glGetString(GL_VENDOR));
	LOGV("GL_RENDERER = %s", glGetString(GL_RENDERER));
	LOGV("GL_SHADING_LANGUAGE_VERSION = %s", glGetString( GL_SHADING_LANGUAGE_VERSION ));

	mMatStack.pushIdentity();

	setViewPort(portW, portH);

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

	LOGV("Compiling shaders");
	LOGV("Compiling basic..");
	mBasicShader.create();
	LOGV("Compiling sky..");
	mSkyShader.create();
	LOGV("Compiling terrain..");
	mTerrainShader.create();
	LOGV("Compiling godray..");
	mGodRayShader.create();
	LOGV("Compiling rect..");
	mRectShader.create();
	LOGV("Compiling particle render..");
	mParticleRenderShader.create();
	LOGV("Compiling particle calculation..");
	mParticleCalcShader.create();

	mGodraySunShader.createFromFiles("shaders/godraysun.vp", "shaders/godraysun.fp");
	mGodraySunShader.bindAttribLocation(0, "inPosition");
	mGodraySunShader.link();



	Utils::makeCube(mCubeBatch, 0.01, BasicShader::VERTEX, BasicShader::TEXTURE_COORDS, BasicShader::NORMALS);

	LOGV("Generating default texture");
	gl( GenTextures(1, &mFontTexture) );
	gl( BindTexture(GL_TEXTURE_2D, mFontTexture) );
	gl( TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE) );
	gl( TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE) );
 
	gl( TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) );
	gl( TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) );


	Buffer<unsigned char> bfr;
	Utils::makeCheckboard(bfr, 100, 100, 5, 5, Color(246/255.0, 2/255.0, 134/255.0), Color::black());
	mInvalidTex.create();
	mInvalidTex.setData(100, 100, GL_RGB, GL_RGB, (const GLbyte*)bfr.getData());

	// font batch
		
	//
	//mShadowFBO.create();
	


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

		mGodrayFBO.bind(gl::FrameBuffer::DRAW);

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
	/* TODO setting light/fog uniforms this way is highly inneficient
		a mechanism of Renderer notification has to be devised
		(dirty flag, event emiting etc..)
	*/
	prog.use();

	// directional
	prog.setUniformVal("uDirectionalLight.ambientItensity",
		scene->getDirectionalLight().mAmbientIntesity);

	prog.setUniformVal("uDirectionalLight.color",
		scene->getDirectionalLight().mColor);

	prog.setUniformVal("uDirectionalLight.diffuseItensity",
		scene->getDirectionalLight().mDiffuseItensity);

	prog.setUniformVal("uDirectionalLight.direction",
		scene->getDirectionalLight().mDirection);

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
		if(scene->getPointLight(i).mActive){
			numActiveLights++;
		}
	}


	prog.setUniformVal("uNumPointLights", (int32_t)numActiveLights);
	for(uint32_t i=0; i<scene->getNumPointLights(); i++){
		if(!scene->getPointLight(i).mActive){
			continue;
		}
		setVal("uPointLights[%d].color", i, scene->getPointLight(i).mColor);
		setVal("uPointLights[%d].ambientItensity", i, scene->getPointLight(i).mAmbientIntesity);
		setVal("uPointLights[%d].diffuseItensity", i, scene->getPointLight(i).mDiffuseItensity);
		setVal("uPointLights[%d].position", i, scene->getPointLight(i).mPosition);

		setVal("uPointLights[%d].attenuation.linear", i, scene->getPointLight(i).mAttenuation.linear);
		setVal("uPointLights[%d].attenuation.constant", i, scene->getPointLight(i).mAttenuation.constant);
		setVal("uPointLights[%d].attenuation.exponential", i, scene->getPointLight(i).mAttenuation.exponential);
	}

	numActiveLights=0;
	for(uint32_t i=0; i<scene->getNumSpotLights(); i++){
		if(scene->getSpotLight(i).mActive){
			numActiveLights++;
		}
	}
	prog.setUniformVal("uNumSpotLights", (int32_t)numActiveLights);

	for(uint32_t i=0; i<scene->getNumSpotLights(); i++){
		if(!scene->getSpotLight(i).mActive){
			continue;
		}

		setVal("uSpotLights[%d].base.color", i, scene->getSpotLight(i).mColor);
		setVal("uSpotLights[%d].base.ambientItensity", i, scene->getSpotLight(i).mAmbientIntesity);
		setVal("uSpotLights[%d].base.diffuseItensity", i, scene->getSpotLight(i).mDiffuseItensity);
		setVal("uSpotLights[%d].base.position", i, scene->getSpotLight(i).mPosition);

		setVal("uSpotLights[%d].base.attenuation.linear", i, scene->getSpotLight(i).mAttenuation.linear);
		setVal("uSpotLights[%d].base.attenuation.constant", i, scene->getSpotLight(i).mAttenuation.constant);
		setVal("uSpotLights[%d].base.attenuation.exponential", i, scene->getSpotLight(i).mAttenuation.exponential);

		setVal("uSpotLights[%d].cutoff", i, cosf(glm::radians(scene->getSpotLight(i).cutoff)));
		setVal("uSpotLights[%d].direction", i, scene->getSpotLight(i).direction);
	}

	prog.setUniformVal("uFogParams.density", scene->getFog().density);
	prog.setUniformVal("uFogParams.color", scene->getFog().color);
	prog.setUniformVal("uEyePos", scene->getCamera().getPosition());

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

void Renderer::render(Scene& scene, SkyBox* sky){
	glm::mat4x4 modelMat;
	sky->getTransform().getMatrix(modelMat);

	glm::mat4x4 viewMat;
	scene.getCamera().getMatrix(viewMat, true);

	mSkyShader.use();
	mSkyShader.setUniformVal("uModelViewProjection",
		mFrustum.getProjMatrix()*(viewMat*modelMat) );

	glActiveTexture(GL_TEXTURE0);
	sky->bind();
	mSkyShader.setUniformVal("uSkyboxTexture", 0);

	sky->getBatch().render();
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

void Renderer::render(Scene& scene, const ModelledActor* actor, PassType pass){
	if(!actor->getModel()){
		return;
	}

	if(actor->getAnimationPlayer()){
		mBasicShader.setUniformVal("uBones",
			actor->getAnimationPlayer()->getBoneMatrices(),
			actor->getAnimationPlayer()->getNumBones()
		);
	}
	
	glm::mat4 modelMat;
	actor->getTransform().getMatrix(modelMat);
	mBasicShader.setModelMatrix(modelMat);

	actor->getModel()->getBatch().bind();

	for(Model::GeometrySkin::MeshList::iterator i=actor->getSkin()->getMeshList().begin(); i!=actor->getSkin()->getMeshList().end(); i++){
		// Use the material if the mesh has one
		Texture2D* texture = i->texture;
		Material* material = &i->material;

		if(material == NULL){
			material = &mDefaultMaterial;
		}

		mBasicShader.setUniformVal("uAlphaTest", material->isAlphaTested());

#ifndef MATERIALS_DISABLED
		setShaderMaterialUniforms(material, mBasicShader);
#endif

		if(texture != NULL){
			texture->bind();
		}
		else{
			mInvalidTex.bind();
		}

		//mBasicShader.setNormalMap( i->normalMap );

		// TODO This might be the solotution but we're not quite there yet (depth peeling ?)
#if 0
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
#else
		/*glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
			GL_ONE, GL_ONE_MINUS_SRC_ALPHA);*/
	/*	glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);*/
#endif
		glDisable(GL_BLEND);
		//glEnable(GL_BLEND);
		//glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
		//glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	/*	if(pass != eNORMAL_PASS){
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
		}*/

		// basically no effect on FPS
		i->geometry->render();
	}
}

/** Terrain node rendering */
void Renderer::render(Scene& scene, const TerrainNode* node){
	// frustum cull test
	if(true /* inFrustum(frustum, node->getBounds() */){
		// have we reached a leaf?
		if(!node->isLeaf()){
			// render children
			for(uint32_t i=0; i<4; i++){
				render(scene, &node->getChildren()[i]);
			}
		}
		else{
			gl( Disable(GL_TEXTURE_2D) );
			node->render();
		}
	}
}

/** Terrain rendering */
void Renderer::render(Scene& scene, const Terrain* terrain, PassType pass){
	
	// Render terrain
	mTerrainShader.use();

	gl::ShaderUniform<int>(&mTerrainShader, "uPassType") = pass;

	setShaderLightUniforms(&scene, mTerrainShader);
	setShaderMaterialUniforms(&mDefaultMaterial, mTerrainShader);

	glm::mat4x4 view;
	scene.getCamera().getMatrix(view);

	mTerrainShader.setMVPMatrix(glm::mat4(1.0), view, mFrustum.getProjMatrix());
	mTerrainShader.setTileTextures( terrain->getTerrainTexture() );
	mTerrainShader.setColorMap( terrain->getMapTexture() );

	render(scene, terrain->getRootNode() );
}

void Renderer::render(Scene& scene, const ParticleEffect* e, PassType pass){
	// No need to render particles in any other pass
	if(pass != eNORMAL_PASS){
		return;
	}

	// TODO fix this

	ParticleEffect* effect = const_cast<ParticleEffect*>(e);

	{
		for(ParticleEffect::LayerMap::iterator i=effect->getLayerMap().begin(); i!=effect->getLayerMap().end(); i++){
			ParticleLayer* layer = i->second;

			// Update particles (TODO move this elsewhere)
			mParticleCalcShader.use();

			// Upload uniforms
			mParticleCalcShader.setUniformVal("uPosition", effect->getTransform().getPosition());
			mParticleCalcShader.setUniformVal("uDt", effect->getTimeDelta());
			mParticleCalcShader.setUniformVal("uSeed", math::random(0, 200));

			mParticleCalcShader.setUniformVal("uMaxLife", layer->getDesc().maxLife);
			mParticleCalcShader.setUniformVal("uMinLife", layer->getDesc().minLife);

			mParticleCalcShader.setUniformVal("uMaxSize", layer->getDesc().maxSize);
			mParticleCalcShader.setUniformVal("uMinSize", layer->getDesc().minSize);

			mParticleCalcShader.setUniformVal("uLocalVelocity", layer->getDesc().localVelocity);
			mParticleCalcShader.setUniformVal("uRandomVelocity", layer->getDesc().randomVelocity);

			mParticleCalcShader.setUniformVal("uEmissionRate", layer->getDesc().emissionRate);

			mParticleCalcShader.setUniformVal("uEmissionVolume", layer->getDesc().emissionVolume);

			

			static Texture2D* rndTex = NULL;
			static bool flag = false;
			if(!flag){
				flag = true;

				rndTex = new Texture2D;
				rndTex->create();

				const uint32_t w = 200;
				const uint32_t h = 200;

				float* data = new float[w*h*3];

				for(int i=0; i<w*h*3; i++){
					data[i] = math::random();
				}

				LOG("CREATE TEXTURE");
				rndTex->setData(w, h, GL_RGB, GL_RGB, (const GLbyte*)data, GL_FLOAT);

				rndTex->dump("rnd-dump.bmp");

				delete[] data;
			}


			gl( ActiveTexture(GL_TEXTURE0) );
			rndTex->bind();

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 

			mParticleCalcShader.setUniformVal("uRandomTexture", 0);

			// WARNING: do NOT enable this sooner, it may cause some serious problems..
			glEnable(GL_RASTERIZER_DISCARD);
			layer->update();
			glDisable(GL_RASTERIZER_DISCARD);
		}

	}
	

	{
		for(ParticleEffect::LayerMap::iterator i=effect->getLayerMap().begin(); i!=effect->getLayerMap().end(); i++){
			ParticleLayer* layer = i->second;

			glm::mat4 view;
			scene.getCamera().getMatrix(view);

			// Render particles

			mParticleRenderShader.use();
			mParticleRenderShader.setUniformVal("uParticleTexture", 0);
			mParticleRenderShader.setUniformVal("uCamPos", scene.getCamera().getPosition());
			mParticleRenderShader.setUniformVal("uSizeGrow", layer->getDesc().sizeGrow);
			mParticleRenderShader.setModelViewProj(view, getFrustum().getProjMatrix());

			for(int i=0; i<ParticleLayer::EffectDesc::kMAX_COLORS; i++){
				char name[64];
				sprintf(name, "uColorAnimation[%d]", i);
				mParticleRenderShader.setUniformVal(name, layer->getDesc().colorAnimation[i]);
			}
		

			// Setup OpenGL
			gl( Enable(GL_BLEND) );
			gl( BlendEquation(GL_FUNC_ADD) );
			gl( BlendFunc(GL_SRC_ALPHA, GL_ONE) );
			gl( DepthMask(false) );


			// Texture
			gl( ActiveTexture(GL_TEXTURE0) );
			if(layer->getDesc().texture){
				layer->getDesc().texture->bind();
			}
			else{
				glBindTexture(GL_TEXTURE_2D, 0);
			}

			layer->render();
		}
	}

	////LOG("####################");
	////LOG("Primitives written %d (to buffer %d)", effect->mPrimittivesWritten, effect->mCurrBatch);
	//for(int k=0; k<2; k++){
	//	LOG("-------\nBuffer %d", k);
	//	ParticleEffect::Particle* particles = (ParticleEffect::Particle*)effect->mBatches[k].getVertexBuffer().map(gl::Buffer::eREAD_WRITE);

	//	for(int i=0; i<effect->getDesc().maxNumber; i++){
	//		ParticleEffect::Particle& particle = particles[i];
	//		int dummy = 3;
	//		dummy += 4;
	//	}

	//	LOG("max life = %f       size=%f", particles[1].maxLife, particles[1].size);


	//	effect->mBatches[k].getVertexBuffer().unmap();
	//}

	gl( DepthMask(true) );
}

void Renderer::render(Scene& scene, PassType pass){
	// OpenGL state
	mNumRenderedTerrainNodes = 0;
	gl( PolygonMode(GL_FRONT_AND_BACK, mRenderState.polygonMode) );

	gl( Enable(GL_DEPTH_TEST) );
	//glDisable(GL_BLEND);

	gl( Viewport(0, 0, mViewPort.x, mViewPort.y) );

	// View matrix
	glm::mat4x4 viewMat;
	scene.getCamera().getMatrix(viewMat);

	
	// Terrain entities
	for(Scene::TerrainSet::const_iterator iter=scene.getTerrainSet().cbegin(); iter!=scene.getTerrainSet().cend(); iter++){
		render(scene, *iter, pass);
	}

	// Modelled actors
	mBasicShader.use();
	gl( ActiveTexture(GL_TEXTURE0) );
	mBasicShader.setUniformVal("uPassType", pass);
	setShaderLightUniforms(&scene, mBasicShader);

#ifdef MATERIALS_DISABLED
	setShaderMaterialUniforms(NULL, mBasicShader);
#endif

	mBasicShader.setMPMatrix(
		viewMat, mFrustum.getProjMatrix());

	gl( Disable(GL_BLEND) );
	gl( Enable(GL_DEPTH_TEST) );
	gl( Disable(GL_CULL_FACE) );

	for(Scene::ModelledActorSet::const_iterator iter=scene.getModelledActors().cbegin(); iter!=scene.getModelledActors().cend(); iter++){
		render(scene, *iter, pass);
	}

	// Sky
	if(pass == eNORMAL_PASS){
		SkyBox* sky = scene.getSkyBox();
		if(sky != NULL){
			render(scene, sky);
		}

		// Particles
		for(Scene::ParticleEffectSet::const_iterator iter=scene.getParticleEffects().cbegin(); iter!=scene.getParticleEffects().cend(); iter++){
			render(scene, *iter, pass);
		}

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
		mGodrayFBO.bind(gl::FrameBuffer::DRAW);

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
		render(scene, eGODRAY_PASS);


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
			gl::FrameBuffer::unbind(gl::FrameBuffer::DRAW);

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
		//setClearColor(Color::red());
		gl( Enable(GL_CULL_FACE) );
		gl( Disable(GL_BLEND) );
		render(scene, eNORMAL_PASS);
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