#ifndef WT_SFSOUNDSYSTEM_H
#define WT_SFSOUNDSYSTEM_H

#include "wt/ASoundSystem.h"

namespace wt
{

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

}; // </SFSoundSystem>

} // </wt>

#endif // </WT_SFSOUNDSYSTEM_H>