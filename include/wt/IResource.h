/**
 * @file IResource.h
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */
 
#ifndef WT_IRESOURCE_H
#define WT_IRESOURCE_H

namespace wt
{

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

}; // </IResource>

} // </wt>

#endif // </WT_IRESOURCE_H>
