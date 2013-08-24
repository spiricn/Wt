#include "wt/stdafx.h"

#include "wt/SFSound.h"
#include "wt/SFSoundBuffer.h"

namespace wt
{

SFSound::SFSound(ASoundBuffer* buffer) : mBuffer(buffer){
	mSound.SetBuffer( static_cast<SFSoundBuffer*>(buffer)->getSFSoundBuffer() );
}

SFSound::~SFSound(){
	mSound.Stop();
}

ASoundBuffer* SFSound::getSoundBuffer(){
	return mBuffer;
}

float SFSound::getVolume() const{
	return mSound.GetVolume()/100.0f;
}

float SFSound::getPitch() const{
	return mSound.GetPitch();
}

void SFSound::setPitch(float pitch){
	mSound.SetPitch(pitch);
}

SFSound::Status SFSound::getStatus() const{
	switch(mSound.GetStatus()){
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
	mSound.Stop();
}

void SFSound::setRelativeToListener(bool state){
	mSound.SetRelativeToListener(state);
}

void SFSound::setVolume(float volume){
	WT_ASSERT(volume >= 0.0f && volume <= 1.0f,
	"Sound volume must be in range [0, 1] got %f", volume);

	mSound.SetVolume(volume * 100.0f);
}

void SFSound::setPosition(const glm::vec3& pos){
	mSound.SetPosition(pos.x, pos.y, pos.z);
}

void SFSound::setLoop(bool state){
	mSound.SetLoop(state);
}

void SFSound::play(){
	mSound.Play();
}

glm::vec3 SFSound::getPosition() const{
	glm::vec3 res;
	sfConvert(mSound.GetPosition(), res);
	return res;
}

void SFSound::setMinimumDistance(float distance){
	mSound.SetMinDistance(distance);
}

void SFSound::setAttenuation(float att){
	mSound.SetAttenuation(att);
}

float SFSound::getMinimumDistance() const{
	return mSound.GetMinDistance();
}

float SFSound::getAttenuation() const{
	return mSound.GetAttenuation();
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