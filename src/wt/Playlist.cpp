#include "wt/stdafx.h"
#include "wt/Playlist.h"
#include "wt/Lua.h"
#define TD_TRACE_TAG "Playlist"

namespace wt
{

Playlist::Playlist() : mCurrentTrack(0){
}

Playlist::~Playlist(){
}

String Playlist::getNext(){
	WT_ASSERT(!mTracks.empty(), "Playlist empty");

	mCurrentTrack = (mCurrentTrack+1) % mTracks.size();

	return getCurrent();
}

String Playlist::getCurrent() const{
	WT_ASSERT(!mTracks.empty(), "Playlist empty");

	return mTracks[mCurrentTrack];
}

void Playlist::addTrack(const String& track){
	mTracks.push_back(track);
}

void Playlist::serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const{
	for(uint32_t i=0; i<mTracks.size(); i++){
		dst.Set(i+1, mTracks[i].c_str());
	}
}

void Playlist::deserialize(lua::State* luaState, const LuaPlus::LuaObject& src){
	mTracks.clear();

	for(LuaPlus::LuaTableIterator iter(src); iter; iter.Next()){
		String track;
		if(lua::luaConv(iter.GetValue(), track)){
			mTracks.push_back(track);
		}
		else{
			WT_THROW("Invalid track path in table of type \"%s\" expected string", iter.GetValue().TypeName());
		}
	}
}

uint32_t Playlist::getNumTracks() const{
	return mTracks.size();
}

bool Playlist::isEmpty() const{
	return mTracks.empty();
}
	
} // </wt>

