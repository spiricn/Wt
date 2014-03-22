#include "wt/stdafx.h"

#include "wt/MusicPlayer.h"

namespace wt
{

MusicPlayer::MusicPlayer(ASoundSystem* soundSystem) : mSoundSystem(soundSystem), mVolume(1.0f), mMuted(false){
}

void MusicPlayer::play(const String& _track){
	String track;
	if(_track.empty()){
		track = mPlaylist.getNext();
	}

	if(mCurrentStream.get() && mCurrentStream->getSource().compare(track) == 0){
		// same track, do nothing
	}
	else{
		mFadeInInt = Interpolator<float>(0, mVolume, 1, false);

		if(mCurrentStream){
			mFadeOutInt = Interpolator<float>(mVolume, 0.0, 1, false);
		}

		mPreviousStream = mCurrentStream;
		mCurrentStream = mSoundSystem->createSoundStream(track);

		mCurrentStream->setVolume(0.0f);
		mCurrentStream->play();
		mCurrentStream->setLoop(true);
	}
}

void MusicPlayer::setVolume(float volume){
	// TODO take into account the new volume if currently fading in or out

	mVolume = volume;
	if(mCurrentStream){
		mCurrentStream->setVolume(volume);
	}
}

void MusicPlayer::onProcEnd(){
	if(mCurrentStream){
		mCurrentStream->stop();
	}

	if(mPreviousStream){
		mPreviousStream->stop();
	}
}

void MusicPlayer::onProcUpdate(float dt){
	if(!mFadeInInt.isFinished()){
		mFadeInInt.update(dt);

		mCurrentStream->setVolume( mFadeInInt.getValue() );
	}

	if(!mFadeOutInt.isFinished()){
		mFadeOutInt.update(dt);

		mPreviousStream->setVolume( mFadeOutInt.getValue() );
	}

	if(mCurrentStream){
		if(!mCurrentStream->isPlaying() && !mPlaylist.isEmpty()){
			// Stopped, play next song
			play();
		}
	}
}

void MusicPlayer::setPlayList(Playlist& playlist){
	mPlaylist = playlist;
}

const Playlist& MusicPlayer::getPlaylist() const{
	return mPlaylist;
}

} // </wt>

