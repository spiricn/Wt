#ifndef WT_ISCENELOADER_H
#define WT_ISCENELOADER_H

#include "wt/AResourceSystem.h"
#include "wt/Scene.h"

namespace wt
{

class ISceneLoader{
public:
	virtual void load(AIOStream& stream) = 0;

	virtual void save(AIOStream& stream) = 0;

	virtual void load(const String& uri) = 0;

	virtual void save(const String& uri) = 0;

}; // </ISceneLoader>

} // </wt>

#endif // </WT_ISCENELOADER_H>
