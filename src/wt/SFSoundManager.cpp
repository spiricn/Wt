#include "wt/stdafx.h"

#include "wt/SFSoundManager.h"
#include "wt/SFSound.h"
#include "wt/SFSoundStream.h"

namespace wt
{

SFSoundManager::SFSoundManager(AResourceSystem* assets) : AResourceManager(assets), mMuted(false){
}

SoundPtr SFSoundManager::createSound(const String& path){
	ASoundBuffer* buffer = getFromPath(path.c_str());
	if(buffer != NULL){
		return new SFSound(buffer);
	}
	else{
		return NULL;
	}
}

SoundPtr SFSoundManager::createSound(ASoundBuffer* bfr){
	return new SFSound(bfr);
}

void SFSoundManager::setListenerPosition(const glm::vec3& pos){
	sf::Listener::SetPosition(pos.x, pos.y, pos.z);
}

void SFSoundManager::setListenerForwardVec(const glm::vec3& fw){
	sf::Listener::SetTarget(fw.x, fw.y, fw.z);
}

SoundStreamPtr SFSoundManager::createSoundStream(const String& path){
	return new SFSoundStream(path);
}

void SFSoundManager::setGlobalVolume(float volume){

	sf::Listener::SetGlobalVolume(volume*100);
}

float SFSoundManager::getGlobalVolume() const{
	return sf::Listener::GetGlobalVolume();
}

bool SFSoundManager::isMuted() const{
	return mMuted;
}

void SFSoundManager::setMuted(bool state){
	mMuted = state;
	setGlobalVolume( state ? 0.0f : 1.0f );
}

ASoundBuffer* SFSoundManager::allocate(AResourceGroup<ASoundBuffer>* parent, const String& resourceName){
	// Generate a new handle
	ResourceHandle handle = newHandle();

	// Allcoate the resource & store it
	ASoundBuffer* rsrc = new SFSoundBuffer(this, handle, resourceName);
	mResources[handle] = rsrc;

	rsrc->setGroup(parent);

	return rsrc;
}

} // </wt>