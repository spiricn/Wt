#include "wt/stdafx.h"

#include "wt/SFSound.h"
#include "wt/SFSoundBuffer.h"

namespace wt
{

SFSound::SFSound(ASoundBuffer* buffer) : mBuffer(buffer){
	mSound.setBuffer( static_cast<SFSoundBuffer*>(buffer)->getSFSoundBuffer() );
}

SFSound::~SFSound(){
	mSound.stop();
}

ASoundBuffer* SFSound::getSoundBuffer(){
	return mBuffer;
}

float SFSound::getVolume() const{
	return mSound.getVolume()/100.0f;
}

float SFSound::getPitch() const{
	return mSound.getPitch();
}

void SFSound::setPitch(float pitch){
	mSound.setPitch(pitch);
}

SFSound::Status SFSound::getStatus() const{
	switch(mSound.getStatus()){
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

void SFSound::stop(){
	mSound.stop();
}

void SFSound::setRelativeToListener(bool state){
	mSound.setRelativeToListener(state);
}

void SFSound::setVolume(float volume){
	WT_ASSERT(volume >= 0.0f && volume <= 1.0f,
	"Sound volume must be in range [0, 1] got %f", volume);

	mSound.setVolume(volume * 100.0f);
}

void SFSound::setPosition(const glm::vec3& pos){
	mSound.setPosition(pos.x, pos.y, pos.z);
}

void SFSound::setLoop(bool state){
	mSound.setLoop(state);
}

void SFSound::play(){
	mSound.play();
}

glm::vec3 SFSound::getPosition() const{
	glm::vec3 res;
	sfConvert(mSound.getPosition(), res);
	return res;
}

void SFSound::setMinimumDistance(float distance){
	mSound.setMinDistance(distance);
}

void SFSound::setAttenuation(float att){
	mSound.setAttenuation(att);
}

float SFSound::getMinimumDistance() const{
	return mSound.getMinDistance();
}

float SFSound::getAttenuation() const{
	return mSound.getAttenuation();
}

void sfConvert(const sf::Vector3f& src, glm::vec3& dst){
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

void sfConvert(const glm::vec3& src, sf::Vector3f& dst){
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

} // </wt>