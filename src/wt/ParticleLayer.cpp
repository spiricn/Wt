#include "wt/stdafx.h"

#include "wt/ParticleLayer.h"

namespace wt{


ParticleLayer::ParticleLayer(ParticleEffect* parent, const String& name, const EffectDesc& desc) : mParent(parent), mName(name), mPrimittivesWritten(0){
	create(desc);
}

ParticleLayer::ParticleLayer(ParticleEffect* parent, const String& name, AResourceSystem* assets, const LuaPlus::LuaObject& src) : mParent(parent), mName(name), mPrimittivesWritten(0){
	create(assets, src);
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

void ParticleLayer::render(){
	if(mPrimittivesWritten > 1){
		mBatches[mCurrBatch ].render(NULL, NULL, 0, 1, mPrimittivesWritten-1);
	}
}

void ParticleLayer::create(const EffectDesc& desc){
	mDesc = desc;
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);

	mCurrBatch = 0;

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

const ParticleLayer::EffectDesc& ParticleLayer::getDesc() const{
	return mDesc;
}

ParticleLayer::EffectDesc::EffectDesc() : 
	localVelocity(0, 0, 0),
	randomVelocity(0, 0, 0),
	emissionVolume(0, 0, 0),
	minLife(4), maxLife(4),
	minSize(10), maxSize(10),
	sizeGrow(0),
	emissionRate(1.0/4),
	particleNumber(200),
	texture(NULL){
		colorAnimation[0] = Color(1, 1, 1, 0);
		colorAnimation[1] = Color::white();
		colorAnimation[2] = Color::white();
		colorAnimation[3] = Color(1, 1, 1, 0.5);
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


void ParticleLayer::serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst){
	// Local velocity
	LuaObject velLocal;
	LUA_NEW_TABLE(velLocal);

	Lua::luaConv(mDesc.localVelocity, velLocal);
	dst.Set("vel.local", velLocal);

	// Random velocity
	LuaObject velRnd;
	LUA_NEW_TABLE(velRnd);

	Lua::luaConv(mDesc.randomVelocity, velRnd);
	dst.Set("vel.rnd", velRnd);

	// emissionVol
	LuaObject emissionVol;
	LUA_NEW_TABLE(emissionVol);

	Lua::luaConv(mDesc.emissionVolume, emissionVol);
	dst.Set("emissionVol", emissionVol);


	dst.Set("life.min", mDesc.minLife);
	dst.Set("life.max", mDesc.maxLife);

	dst.Set("size.min", mDesc.minSize);
	dst.Set("size.max", mDesc.maxSize);
	dst.Set("size.grow", mDesc.sizeGrow);

	dst.Set("emissionRate", mDesc.emissionRate);

	dst.Set("particleNum", mDesc.particleNumber);

	if(mDesc.texture){
		dst.Set("texture", mDesc.texture->getPath().c_str());
	}
	else{
		dst.Set("texture", 0);
	}

	LuaObject colorAni;
	LUA_NEW_TABLE(colorAni);

	for(uint32_t i=0; i<mDesc.kMAX_COLORS; i++){
		LuaObject color;
		LUA_NEW_TABLE(color);

		Lua::luaConv(mDesc.colorAnimation[i], color);

		colorAni.Set(i, color);
	}

	dst.Set("color.anim", colorAni);
}

void ParticleLayer::create(AResourceSystem* assets, const LuaPlus::LuaObject& src){
	EffectDesc desc;

	Lua::luaConv(src.Get("vel.local"), desc.localVelocity);
	Lua::luaConv(src.Get("vel.rnd"), desc.randomVelocity);
	Lua::luaConv(src.Get("emissionVol"), desc.emissionVolume);
	Lua::luaConv(src.Get("life.min"), desc.minLife);
	Lua::luaConv(src.Get("life.max"), desc.maxLife);
	Lua::luaConv(src.Get("size.min"), desc.minSize);
	Lua::luaConv(src.Get("size.max"), desc.maxSize);
	Lua::luaConv(src.Get("size.grow"), desc.sizeGrow);
	Lua::luaConv(src.Get("emissionRate"), desc.emissionRate);
	Lua::luaConv(src.Get("particleNum"), desc.particleNumber);

	String texPath;
	LuaObject luaTexPath = src.Get("texture");
	if(luaTexPath.IsString()){
		Lua::luaConv(luaTexPath, texPath);
		desc.texture = assets->getTextureManager()->getFromPath(texPath);
	}

	if(!desc.texture){
		WT_THROW("Error deserializing particle effect - unable to find texture uri=\"%s\"", texPath.c_str());
	}

	
	for(uint32_t i=0; i<EffectDesc::kMAX_COLORS; i++){
		LuaObject& colorAnim = src.Get("color.anim");

		Lua::luaConv(colorAnim.Get(i+1), desc.colorAnimation[i]);
	}

	create(desc);
}

}; // </wt>

