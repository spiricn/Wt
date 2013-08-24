#include "wt/stdafx.h"

#include "wt/Sound.h"

#define TD_TRACE_TAG "Sound"


namespace wt
{

Sound::Sound(Scene* parent, uint32_t id, const String& name) : ASceneActor(parent, ASceneActor::eTYPE_SOUND, id, name){
}

ATransformable* Sound::getTransformable(){
	return this;
}

SoundPtr Sound::getSound() const{
	return mSound;
}

void Sound::setSound(SoundPtr sound){
	mSound = sound;
}

void Sound::update(float dt){
	ASceneActor::update(dt);
}

void Sound::setTranslation(const glm::vec3& translation){
	if(mSound.get() != NULL){
		mSound->setPosition(translation);
	}
}

void Sound::setRotation(const glm::quat& /*rotation*/){
}

void Sound::setScale(const glm::vec3& /*scale*/){
}

void Sound::getScale(glm::vec3& /*result*/) const{

}

void Sound::getTranslation(glm::vec3& result) const{
	if(mSound != NULL){
		result = mSound->getPosition();
	}
}

void Sound::getRotation(glm::quat& /*result*/) const{
}

void Sound::serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst, void* opaque){
	ASceneActor::serialize(assets, dst, opaque);

	if(mSound.get() != NULL){
		dst.Set("rsrc", mSound->getSoundBuffer()->getPath().c_str());
	}
	else{
		dst.Set("rsrc", 0);
	}
}

void Sound::deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src, void* opaque){
	String rsrcPath;
	if(lua::luaConv(src.Get("rsrc"), rsrcPath)){
		mSound = assets->getSoundSystem()->createSound(rsrcPath);

		mSound->setRelativeToListener(false);
		mSound->setLoop(true);
		mSound->play();
	}

	ASceneActor::deserialize(assets, src, opaque);
}

} // </wt>