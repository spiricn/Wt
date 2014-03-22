#ifndef WT_PLAYLIST_H
#define WT_PLAYLIST_H

#include <wt/ASerializable.h>

namespace wt
{

class Playlist : public lua::ASerializable{
public:
	Playlist();

	virtual ~Playlist();

	String getNext();

	String getCurrent() const;

	void addTrack(const String& track);

	void serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const;

	void deserialize(lua::State* luaState, const LuaPlus::LuaObject& src);

	uint32_t getNumTracks() const;

	bool isEmpty() const;

private:
	StringList mTracks;
	uint32_t mCurrentTrack;

}; // </Playlist>

} // </wt>

#endif // </WT_PLAYLIST_H>
