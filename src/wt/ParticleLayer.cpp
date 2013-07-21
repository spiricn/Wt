#include "wt/stdafx.h"

#include "wt/ParticleLayer.h"

namespace wt{


ParticleLayer::ParticleLayer(ParticleEffect* parent, const String& name, ParticleLayerResource* layerRsrc) : mParent(parent), mName(name), mLayerRsrc(NULL){
	create(layerRsrc);
}

ParticleLayer::~ParticleLayer(){
}

void ParticleLayer::setName(const String& name){
	mName = name;
}

const String& ParticleLayer::getName() const{
	return mName;
}

ParticleEffect* ParticleLayer::getParent() const{
	return mParent;
}

ParticleLayerResource* ParticleLayer::getLayerResource() const{
	return mLayerRsrc;
}

void ParticleLayer::render(){
	if(mPrimittivesWritten > 1){
		mBatches[mCurrBatch ].render(NULL, NULL, 0, 1, mPrimittivesWritten-1);
	}
}


// TODO rsrc should be const
void ParticleLayer::create(ParticleLayerResource* rsrc){

	mLayerRsrc = rsrc;

	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);
	
	mCurrBatch = 0;

	const ParticleLayerResource::LayerDesc& desc = rsrc->getDesc();

	// Initialize particles
	Particle* particles = new Particle[desc.particleNumber];
	memset(particles, 0x00, sizeof(Particle)*desc.particleNumber);

	mPrimittivesWritten = desc.particleNumber;

	// Initialize emitter
	particles[0].type = 1.0f;
	particles[0].life = 0.0f;


	// Create two batches for double-buffered transform feedback
	for(uint32_t i=0; i<2; i++){
		mBatches[i].create(
			GL_POINTS,
			i == 0 ? particles : NULL,
			desc.particleNumber, sizeof(Particle)
		);

		// type
		mBatches[i].setVertexAttribute(
			ParticleCalculationShader::eATTR_TYPE, 1, GL_FLOAT, offsetof(Particle, type));
		// position
		mBatches[i].setVertexAttribute(
			ParticleCalculationShader::eATTR_POSITION, 3, GL_FLOAT, offsetof(Particle, x));
		// life
		mBatches[i].setVertexAttribute(
			ParticleCalculationShader::eATTR_LIFE, 1, GL_FLOAT, offsetof(Particle, life));
		// velocity
		mBatches[i].setVertexAttribute(
			ParticleCalculationShader::eATTR_VELOCITY, 3, GL_FLOAT, offsetof(Particle, vx));
		// size
		mBatches[i].setVertexAttribute(
			ParticleCalculationShader::eATTR_SIZE, 1, GL_FLOAT, offsetof(Particle, size));
		// max life
		mBatches[i].setVertexAttribute(
			ParticleCalculationShader::eATTR_MAX_LIFE, 1, GL_FLOAT, offsetof(Particle, maxLife));
	}

	delete[] particles;
}

void ParticleLayer::update(){
	mQuery.begin(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

	mBatches[mCurrBatch].render( &mBatches[(mCurrBatch+1)%2].getVertexBuffer(), NULL, 0, 0, mPrimittivesWritten);
	mCurrBatch = (mCurrBatch+1)%2;

	 //Query the number of primitives written in the transform buffer.
	mQuery.getResult(&mPrimittivesWritten);
}

void ParticleCalculationShader::create(){
	gl::ShaderProgram::createFromFiles("shaders/particle.vp", "", "shaders/particle_gp.vp");

	bindAttribLocation(eATTR_TYPE, "inType");
	bindAttribLocation(eATTR_POSITION, "inPosition");
	bindAttribLocation(eATTR_LIFE, "inLife");
	bindAttribLocation(eATTR_VELOCITY, "inVelocity");
	bindAttribLocation(eATTR_SIZE, "inSize");
	bindAttribLocation(eATTR_MAX_LIFE, "inMaxLife");

	setTransformFeedbackVaryings(6,
		"outType", "outPosition", "outLife", "outVelocity", "outSize", "outMaxLife"
	);

	link();
}

void ParticleRenderShader::setModelViewProj(const glm::mat4& modelView, const glm::mat4& proj){
	setUniformVal("uModelViewMat", modelView);
	setUniformVal("uProjMat", proj);
}


void ParticleRenderShader::create(){
	gl::ShaderProgram::createFromFiles("shaders/particle_render.vp", "shaders/particle.fp");

	bindAttribLocation(ParticleCalculationShader::eATTR_TYPE, "inType");
	bindAttribLocation(ParticleCalculationShader::eATTR_POSITION, "inPosition");
	bindAttribLocation(ParticleCalculationShader::eATTR_LIFE, "inLife");
	bindAttribLocation(ParticleCalculationShader::eATTR_VELOCITY, "inVelocity");
	bindAttribLocation(ParticleCalculationShader::eATTR_SIZE, "inSize");
	bindAttribLocation(ParticleCalculationShader::eATTR_MAX_LIFE, "inMaxLife");

	link();
}


}; // </wt>

