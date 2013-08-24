#include "wt/stdafx.h"

#include "wt/ASoundSystem.h"

#define TD_TRACE_TAG "ASoundSystem"

namespace wt
{

void ASoundSystem::setManager(AResourceManager<ASoundBuffer>* manager){
	mSoundManager = manager;
}

SoundPtr ASoundSystem::playSound(const String& path){
	return playSound( createSound(path) );
}

SoundPtr ASoundSystem::playSound(SoundPtr sound){
	sound->play();
	mActiveSounds.push_back(sound);
	return sound;
}

void ASoundSystem::update(float dt){
	for(std::vector<SoundPtr>::iterator i=mActiveSounds.begin(); i!=mActiveSounds.end(); i++){
		if((*i)->getStatus() == ASound::eSTOPPED){
			i = mActiveSounds.erase(i);
		}

		if(i==mActiveSounds.end()){
			break;
		}
	}
}

ASoundSystem::~ASoundSystem(){
}


} // </wt>