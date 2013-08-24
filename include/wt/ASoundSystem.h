#ifndef WT_SOUNDSYSTEM_H
#define WT_SOUNDSYSTEM_H

#include "wt/ASound.h"
#include "wt/Sp.h"
#include "wt/Singleton.h"
#include "wt/EventManager.h"
#include "wt/AResourceManager.h"
#include "wt/Lua.h"
#include "wt/ASoundStream.h"

namespace wt{

typedef Sp<ASound> SoundPtr;
typedef Sp<ASoundStream> SoundStreamPtr;

class ASoundSystem{
public:
	virtual ~ASoundSystem();

	void setManager(AResourceManager<ASoundBuffer>* manager);

	SoundPtr playSound(const String& path);

	SoundPtr playSound(SoundPtr sound);

	void update(float dt);

	virtual SoundPtr createSound(const String& path)=0;

	virtual SoundPtr createSound(ASoundBuffer* bfr)=0;

	virtual SoundStreamPtr createSoundStream(const String& path)=0;

	virtual void setListenerPosition(const glm::vec3& position) = 0;

	virtual void setListenerForwardVec(const glm::vec3& fws) = 0;

	/** range [0.0f, 1.0f] */
	virtual void setGlobalVolume(float volume) = 0;

	/** range [0.0f, 1.0f] */
	virtual float getGlobalVolume() const = 0;

	virtual bool isMuted() const = 0;

	virtual void setMuted(bool state) = 0;

private:
	AResourceManager<ASoundBuffer>* mSoundManager;

protected:
	std::vector<SoundPtr> mActiveSounds;

}; // </ASoundSystem>

} // </wt>

#endif