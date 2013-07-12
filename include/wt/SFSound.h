#ifndef WT_SFSOUND_H
#define WT_SFSOUND_H

#include "wt/stdafx.h"

#include "wt/AResourceManager.h"
#include "wt/ASound.h"
#include "wt/ASoundSystem.h"

namespace wt{

class SFSoundBuffer : public ASoundBuffer{
private:
	sf::SoundBuffer mSoundBuffer;

public:
	sf::SoundBuffer& getSFSoundBuffer(){
		return mSoundBuffer;
	}
	
	SFSoundBuffer(ResourceHandle handle=0, const String& name="") : ASoundBuffer(handle, name){
	}

}; // </ASoundBuffer>


inline void sfConvert(const sf::Vector3f& src, glm::vec3& dst){
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

inline void sfConvert(const glm::vec3& src, sf::Vector3f& dst){
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

class SFSound : public ASound{
private:
	sf::Sound mSound;
public:
	SFSound(ASoundBuffer* buffer){
		mSound.SetBuffer( static_cast<SFSoundBuffer*>(buffer)->getSFSoundBuffer() );
	}

	~SFSound(){
		mSound.Stop();
	}

	Status getStatus() const{
		switch(mSound.GetStatus()){
		case sf::Sound::Playing:
			return ePLAYING;
		case sf::Sound::Stopped:
			return eSTOPPED;
		case sf::Sound::Paused:
			return ePAUSED;
		default:
			return eSTOPPED;
		}
	}

	void stop(){
		mSound.Stop();
	}

	void setRelativeToListener(bool state){
		mSound.SetRelativeToListener(state);
	}

	void setVolume(float volume){
		WT_ASSERT(volume >= 0.0f && volume <= 1.0f,
		"Sound volume must be in range [0, 1] got %f", volume);

		mSound.SetVolume(volume * 100.0f);
	}

	void setPosition(const glm::vec3& pos){
		mSound.SetPosition(pos.x, pos.y, pos.z);
	}

	void setLoop(bool state){
		mSound.SetLoop(state);
	}

	void play(){
		mSound.Play();
	}

	glm::vec3 getPosition() const{
		glm::vec3 res;
		sfConvert(mSound.GetPosition(), res);
		return res;
	}

};

class SFSoundManager : public AResourceManager<ASoundBuffer>{
private:

	ASoundBuffer* allocate(AResourceGroup<ASoundBuffer>* parent, const String& resourceName){
		// Generate a new handle
		ResourceHandle handle = newHandle();

		// Allcoate the resource & store it
		ASoundBuffer* rsrc = new SFSoundBuffer(handle, resourceName);
		mResources[handle] = rsrc;
		rsrc->setGroup(parent);
		return rsrc;
	}
};

class SFSoundLoader: public AResourceLoader<ASoundBuffer>, public Singleton<SFSoundLoader>{
public:
	void load(const String& path, ASoundBuffer* dst){
		SFSoundBuffer* bfr = static_cast<SFSoundBuffer*>(dst);

		if(!bfr->getSFSoundBuffer().LoadFromFile(path)){
			WT_EXCEPT("SoundBufferLoader",
				"Error loading sound \"%s\" from \"%s\"", bfr->getName().c_str(),
				path.c_str());
		}

		//dst->setUri(path);
	}
};


class SFSoundStream : public ASoundStream{
private:
	sf::Music mMusic;
public:
	SFSoundStream(const String& src){
		setSource(src);
	}

	~SFSoundStream(){
		mMusic.Stop();
	}

	void setRelativeToListener(bool state){
		mMusic.SetRelativeToListener(state);
	}

	void setSource(const String& src){
		ASoundStream::setSource(src);

		if(!mMusic.OpenFromFile(src)){
			WT_EXCEPT("SFSoundStream",
				"Error openning sound stream from \"%s\"", src.c_str());
		}
	}

	virtual Status getStatus() const{
		switch( mMusic.GetStatus() ){
		case sf::Sound::Stopped:
			return eSTOPPED;
		case sf::Sound::Playing:
			return ePLAYING;
		case sf::Sound::Paused:
			return ePAUSED;
		default:
			return eSTOPPED;
		}
	}

	virtual void play(){
		mMusic.Play();
	}

	void setVolume(float volume){
		WT_ASSERT(volume >= 0.0f && volume <= 1.0f,
			"Sound volume must be in range [0, 1] got %f", volume);

		mMusic.SetVolume(volume * 100.0f);
	}

	virtual void setLoop(bool state){
		mMusic.SetLoop(state);
	}

	virtual void stop(){
		mMusic.Stop();
	}

	virtual void setPosition(const glm::vec3& position){
	}

	virtual glm::vec3 getPosition() const{
		static glm::vec3 tmp;
		// TODO
		return tmp;
	}
}; // </SFSoundStream>

class SFSoundSystem : public ASoundSystem{
private:
	bool mIsMuted;
	SFSoundManager* mSoundManager;

public: 
	
	SFSoundSystem(SFSoundManager* soundManager) : mIsMuted(false), mSoundManager(soundManager){
	}

	SoundPtr createSound(const String& path){
		return new SFSound(mSoundManager->getFromPath(path.c_str()));
	}

	virtual SoundPtr createSound(ASoundBuffer* bfr){
		return new SFSound(bfr);
	}

	void setListenerPosition(const glm::vec3& pos){
		sf::Listener::SetPosition(pos.x, pos.y, pos.z);
	}

	void setListenerForwardVec(const glm::vec3& fw){
		sf::Listener::SetTarget(fw.x, fw.y, fw.z);
	}

	SoundStreamPtr createSoundStream(const String& path){
		return new SFSoundStream(path);
	}

	void setGlobalVolume(float volume){

		sf::Listener::SetGlobalVolume(volume*100);
	}

	float getGlobalVolume() const{
		return sf::Listener::GetGlobalVolume();
	}

	virtual bool isMuted() const{
		return mIsMuted;
	}

	virtual void setMuted(bool state){
		mIsMuted = state;
		setGlobalVolume( state ? 0.0f : 1.0f );
	}
};


}; // </wt>

#endif