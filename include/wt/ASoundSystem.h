#ifndef WT_SOUNDSYSTEM_H
#define WT_SOUNDSYSTEM_H

#include "wt/ASound.h"
#include "wt/Sp.h"
#include "wt/Singleton.h"
#include "wt/EventManager.h"
#include "wt/AResourceManager.h"

namespace wt{

typedef Sp<ASound> SoundPtr;
typedef Sp<ASoundStream> SoundStreamPtr;

class SetGlobalVolumeEvt : public Event{
public:
	static const EvtType TYPE;

protected:
	void serialize(LuaObject& dst){
		dst.Set("volume", mVolume);
	}

	void deserialize(LuaObject& src){
		mVolume = src.Get("volume").ToNumber();
	}

public:
	float mVolume;

	SetGlobalVolumeEvt(float volume) : mVolume(volume){
	}

	SetGlobalVolumeEvt(LuaObject& data) : Event(data){
		deserialize(data);
	}

	const EvtType& getType() const{
		return TYPE;
	}

}; // </SetGlobalVolumeEvt>

#define EXPOSE(funcName) do{ meta.RegisterObjectDirect(#funcName, (ASoundSystem*)0, &ASoundSystem::lua_ ## funcName); }while(0)

class ASoundSystem : public EventListener, public ALuaObject{

private:
	AResourceManager<ASoundBuffer>* mSoundManager;

protected:
	bool handleEvent(const Sp<Event> e){
		const EvtType& type = e->getType();

		if(type == SetGlobalVolumeEvt::TYPE){
			setGlobalVolume( static_cast<const SetGlobalVolumeEvt*>(e.get())->mVolume );
		}

		return true;
	}


	std::vector<SoundPtr> mActiveSounds;
public:
	void setManager(AResourceManager<ASoundBuffer>* manager){
		mSoundManager = manager;
	}

	SoundPtr playSound(const String& path){
		return playSound( createSound(path) );
	}

	SoundPtr playSound(SoundPtr sound){
		sound->play();
		mActiveSounds.push_back(sound);
		return sound;
	}

	void update(float dt){
		for(std::vector<SoundPtr>::iterator i=mActiveSounds.begin(); i!=mActiveSounds.end(); i++){
			if((*i)->getStatus() == ASound::eSTOPPED){
				i = mActiveSounds.erase(i);
			}

			if(i==mActiveSounds.end()){
				break;
			}
		}
	}

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

	void registerEvents(){
#if 0
		// TODO
		EventManager::getSingleton().registerSharedEvent<SetGlobalVolumeEvt>(SetGlobalVolumeEvt::TYPE);
		EventManager::getSingleton().registerListener(this, SetGlobalVolumeEvt::TYPE);
#endif
	}

	/**********************/
	/**** Lua bindings ****/
	/**********************/




	void lua_playSound(LuaObject soundPath, LuaObject luaPos){
		SoundPtr sound = playSound(soundPath.ToString());
		if(!luaPos.IsNil()){
			glm::vec3 pos;
			if(Lua::luaConv(luaPos, pos)){
				//sound.
				sound->setPosition( pos );
				sound->setRelativeToListener(false);
			}
		
		}
	}


	void expose(LuaObject& meta){
		EXPOSE(playSound);
	}

}; // </ASoundSystem>

#undef EXPOSE

} // </wt>

#endif