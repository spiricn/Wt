/**
 * @file IResourceManager.h
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */
 
#ifndef WT_IRESOURCEMANAGER_H
#define WT_IRESOURCEMANAGER_H

#include "wt/IResource.h"

namespace wt
{

class IResourceManager{
public:
	virtual ~IResourceManager(){
	}

	virtual IResource::ResourceType getResourceType() = 0;

	virtual void deserialize(lua::State* luaState, const LuaPlus::LuaObject& table, IResourceSet& set) = 0;

	virtual void serialize(lua::State* luaState, LuaPlus::LuaObject& table, IResourceSet& set) = 0;

	virtual void loadSet(IResourceSet& set) = 0;

	virtual void createSet(IResourceSet& set) = 0;

	virtual void destroySet(IResourceSet& set) = 0;

}; // </IResourceManager>

} // </wt>

#endif // </WT_IRESOURCEMANAGER_H>
