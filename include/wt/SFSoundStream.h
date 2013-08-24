#ifndef WT_SFSOUNDSTREAM_H
#define WT_SFSOUNDSTREAM_H

#include "wt/ASoundStream.h"

namespace wt
{

class SFSoundStream : public ASoundStream{
public:
	SFSoundStream(const String& src);

	~SFSoundStream();

	void setRelativeToListener(bool state);

	void setSource(const String& src);

	Status getStatus() const;

	void play();

	void setVolume(float volume);

	void setLoop(bool state);

	void stop();

	void setPosition(const glm::vec3& position);

	glm::vec3 getPosition() const;

	void setMinimumDistance(float distance);

	void setAttenuation(float att);

	float getMinimumDistance() const;

	float getAttenuation() const;

	float getVolume() const;

	float getPitch() const;

	void setPitch(float pitch) ;

private:
	sf::Music mMusic;

}; // </SFSoundStream>

} // </wt>

#endif // </WT_SFSOUNDSTREAM_H>