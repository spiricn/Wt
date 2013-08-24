#ifndef WT_SFSOUNDMANAGER_H
#define WT_SFSOUNDMANAGER_H

#include "wt/SFSoundBuffer.h"
#include "wt/AResourceManager.h"
#include "wt/ASoundSystem.h"

namespace wt
{

class SFSoundManager : public AResourceManager<ASoundBuffer>, public ASoundSystem{
public:
	SFSoundManager(AResourceSystem* assets);

	SoundPtr createSound(const String& path);

	SoundPtr createSound(ASoundBuffer* bfr);

	void setListenerPosition(const glm::vec3& pos);

	void setListenerForwardVec(const glm::vec3& fw);

	SoundStreamPtr createSoundStream(const String& path);

	void setGlobalVolume(float volume);

	float getGlobalVolume() const;

	bool isMuted() const;

	void setMuted(bool state);

private:
	ASoundBuffer* allocate(AResourceGroup<ASoundBuffer>* parent, const String& resourceName);
	bool mMuted;

}; // </SFSoundManager>

} // </wt>

#endif // </WT_SFSOUNDMANAGER_H>