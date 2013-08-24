#ifndef WT_SFSOUND_H
#define WT_SFSOUND_H

#include "wt/stdafx.h"

#include "wt/AResourceManager.h"
#include "wt/ASound.h"
#include "wt/ASoundSystem.h"

namespace wt{

class SFSound : public ASound{
public:
	SFSound(ASoundBuffer* buffer);

	~SFSound();

	ASoundBuffer* getSoundBuffer();

	Status getStatus() const;

	void stop();

	void setRelativeToListener(bool state);

	void setVolume(float volume);

	void setPosition(const glm::vec3& pos);

	void setLoop(bool state);

	void play();

	glm::vec3 getPosition() const;

	void setMinimumDistance(float distance);

	void setAttenuation(float att);

	float getMinimumDistance() const;

	float getAttenuation() const;

	float getVolume() const;

	float getPitch() const;

	void setPitch(float pitch);

private:
	sf::Sound mSound;
	ASoundBuffer* mBuffer;

}; // </SFSound>

void sfConvert(const sf::Vector3f& src, glm::vec3& dst);

void sfConvert(const glm::vec3& src, sf::Vector3f& dst);

}; // </wt>

#endif