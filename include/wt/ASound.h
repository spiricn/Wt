#ifndef WT_ASOUND_H
#define WT_ASOUND_H

#include "wt/AResource.h"
#include "wt/lua/Object.h"
#include "wt/ASoundBuffer.h"

namespace wt{

class ASound : public lua::Object<ASound>{
public:
	enum Status{
		ePLAYING,
		ePAUSED,
		eSTOPPED
	};

public:
	virtual ~ASound(){
	}

	virtual Status getStatus() const = 0;

	virtual void play() = 0;

	virtual void setLoop(bool state) = 0;

	virtual void stop() = 0;

	/** range [0.0f, 1.0f] */
	virtual void setVolume(float volume) = 0;

	virtual void setPosition(const glm::vec3& position) = 0;

	virtual glm::vec3 getPosition() const = 0;

	virtual bool isPlaying() const{
		return getStatus() == ePLAYING;
	}

	virtual void setRelativeToListener(bool state) = 0;

	void generateMetaTable(){
		expose("play", &ASound::play);
	}

	virtual ASoundBuffer* getSoundBuffer() = 0;

	/**
	 * Sets the minimum distance under which the sound will be heard at its maximum volume
	 * MinDistance is the sound minimum distance, set with SetMinDistance
	 * Attenuation is the sound attenuation, set with SetAttenuation
	 * Distance    is the distance between the sound and the listener
     *
	 * Factor = MinDistance / (MinDistance + Attenuation * (max(Distance, MinDistance) - MinDistance))
	 */
	virtual void setMinimumDistance(float distance) = 0;

	virtual void setAttenuation(float att) = 0;

	virtual float getMinimumDistance() const = 0;

	virtual float getAttenuation() const = 0;

	virtual float getVolume() const = 0;

	virtual float getPitch() const = 0;

	virtual void setPitch(float pitch) = 0;

}; // </ASound>

}; // </wt>


#endif