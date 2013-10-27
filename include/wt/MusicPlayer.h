#ifndef WT_MUSICPLAYER_H
#define WT_MUSICPLAYER_H

#include "wt/ASoundSystem.h"
#include "wt/ProcessManager.h"
#include "wt/Interpolator.h"

namespace wt{


class Playlist{
private:
	StringList mTracks;
	uint32_t mCurrentTrack;

public:
	Playlist() : mCurrentTrack(0){
	}

	String getNext(){
		mCurrentTrack = (mCurrentTrack+1) % mTracks.size();
		if(!mTracks.empty()){
			return mTracks[mCurrentTrack];
		}
	}

	void serialize(LuaObject& dst){
		for(uint32_t i=0; i<mTracks.size(); i++){
			dst.Set(i+1, mTracks[i].c_str());
		}
	}

	void deserialize(const LuaObject& src){
		mTracks.clear();

		for(LuaTableIterator iter(src); iter; iter.Next()){
			String track;
			if(lua::luaConv(iter.GetValue(), track)){
				mTracks.push_back(track);
			}
			else{
				WT_THROW("Invalid track path in table of type \"%s\" expected string", iter.GetValue().TypeName());
			}
		}
	}

}; // </Playlist>

class MusicPlayer : public AProcess{
private:
	SoundStreamPtr mCurrentStream, mPreviousStream;
	Interpolator<float> mFadeInInt, mFadeOutInt;
	ASoundSystem* mSoundSystem;
	float mVolume;
	bool mMuted;
	Playlist* mPlaylist;

public:
	MusicPlayer(ASoundSystem* soundSystem) : mSoundSystem(soundSystem), mVolume(1.0f), mMuted(false), mPlaylist(NULL){
	}

	void play(const String& track){
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

	void setVolume(float volume){
		mVolume = volume;
		if(mCurrentStream){
			mCurrentStream->setVolume(volume);
		}
	}

	void onProcEnd(){
		if(mCurrentStream){
			mCurrentStream->stop();
		}

		if(mPreviousStream){
			mPreviousStream->stop();
		}
	}

	void onProcUpdate(float dt){
		if(!mFadeInInt.isFinished()){
			mFadeInInt.update(dt);

			mCurrentStream->setVolume( mFadeInInt.getValue() );
		}

		if(!mFadeOutInt.isFinished()){
			mFadeOutInt.update(dt);

			mPreviousStream->setVolume( mFadeOutInt.getValue() );
		}
	}

}; // </MusicPlayer>

}; // </wt>

#endif // </WT_MUSICPLAYER_H>
