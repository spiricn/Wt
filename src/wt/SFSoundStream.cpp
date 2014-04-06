#include "wt/stdafx.h"

#include "wt/SFSoundStream.h"
#include "wt/Exception.h"

namespace wt
{

SFSoundStream::SFSoundStream(const String& src){
	setSource(src);
}

SFSoundStream::~SFSoundStream(){
	mMusic.stop();
}

void SFSoundStream::setRelativeToListener(bool state){
	mMusic.setRelativeToListener(state);
}

void SFSoundStream::setSource(const String& src){
	ASoundStream::setSource(src);

	if(!mMusic.openFromFile(src)){
		WT_THROW(
			"Error openning sound stream from \"%s\"", src.c_str());
	}
}

float SFSoundStream::getPitch() const{
	return mMusic.getPitch();
}

void SFSoundStream::setPitch(float pitch){
	mMusic.setPitch(pitch);
}

float SFSoundStream::getVolume() const{
	return mMusic.getVolume();
}

SFSoundStream::Status SFSoundStream::getStatus() const{
	switch( mMusic.getStatus() ){
	case sf::Sound::Stopped:
		return eSTOPPED;
	case sf::Sound::Playing:
		return ePLAYING;
	case sf::Sound::Paused:
		return ePAUSED;
	default:
		return eSTOPPED;
	}
}

void SFSoundStream::play(){
	mMusic.play();
}

void SFSoundStream::setVolume(float volume){
	WT_ASSERT(volume >= 0.0f && volume <= 1.0f,
		"Sound volume must be in range [0, 1] got %f", volume);

	mMusic.setVolume(volume * 100.0f);
}

void SFSoundStream::setLoop(bool state){
	mMusic.setLoop(state);
}

void SFSoundStream::stop(){
	mMusic.stop();
}

void SFSoundStream::setPosition(const glm::vec3& position){
	WT_THROW("NOT IMPLEMENTED");
}

glm::vec3 SFSoundStream::getPosition() const{
	WT_THROW("NOT IMPLEMENTED");
}

void SFSoundStream::setMinimumDistance(float distance){
	WT_THROW("NOT IMPLEMENTED");
}

void SFSoundStream::setAttenuation(float att){
	WT_THROW("NOT IMPLEMENTED");
}

float SFSoundStream::getMinimumDistance() const{
	WT_THROW("NOT IMPLEMENTED");
}

float SFSoundStream::getAttenuation() const{
	WT_THROW("NOT IMPLEMENTED");
}


} // </wt>