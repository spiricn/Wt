/**
 * @file IResource.h
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */
 
#ifndef WT_IRESOURCE_H
#define WT_IRESOURCE_H

namespace wt
{

typedef unsigned long ResourceHandle;

class IResource{
public:
	enum ResourceType{
		eTYPE_INVALID,

		eTYPE_IMAGE,
		eTYPE_TEXTURE,
		eTYPE_FONT,
		eTYPE_MODEL,
		eTYPE_SKYBOX,
		eTYPE_ANIMATION,
		eTYPE_SOUND,
		eTYPE_PARTICLE,
		eTYPE_HEIGHTMAP,
		eTYPE_SCRIPT,

		eTYPE_MAX,
	}; // </ResourceType>

public:
	virtual ResourceType getResourceType() = 0;

	virtual String getPath() const = 0;

	virtual ResourceHandle getHandle() const = 0;

	virtual const String& getName() const = 0;

	virtual void serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const = 0;

	virtual void deserialize(lua::State* luaState, const LuaPlus::LuaObject& table) = 0;

}; // </IResource>

typedef std::set<IResource*> IResourceSet;

} // </wt>

#endif // </WT_IRESOURCE_H>
