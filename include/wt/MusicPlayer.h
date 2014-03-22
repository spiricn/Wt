#ifndef WT_MUSICPLAYER_H
#define WT_MUSICPLAYER_H

#include "wt/ASoundSystem.h"
#include "wt/ProcessManager.h"
#include "wt/Interpolator.h"
#include "wt/Playlist.h"

namespace wt
{

class MusicPlayer : public AProcess{
public:
	MusicPlayer(ASoundSystem* soundSystem);

	void play(const String& track="");

	void pause();

	void unpause();

	void setVolume(float volume);

	void onProcEnd();

	void onProcUpdate(float dt);

	void setPlayList(Playlist& playlist);

	const Playlist& getPlaylist() const;

private:
	SoundStreamPtr mCurrentStream, mPreviousStream;
	Interpolator<float> mFadeInInt, mFadeOutInt;
	ASoundSystem* mSoundSystem;
	float mVolume;
	bool mMuted;
	Playlist mPlaylist;
}; // </MusicPlayer>

}; // </wt>

#endif // </WT_MUSICPLAYER_H>
