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
	virtual IResource::ResourceType getResourceType() = 0;

}; // </IResourceManager>

} // </wt>

#endif // </WT_IRESOURCEMANAGER_H>
