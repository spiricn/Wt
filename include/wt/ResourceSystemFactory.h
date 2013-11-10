#ifndef WT_RESOURCESYSTEMFACTORY_H
#define WT_RESOURCESYSTEMFACTORY_H

#include "wt/AResourceSystem.h"

namespace wt
{

class ResourceSystemFactory{
public:

	static AResourceSystem* create(const AResourceSystem::Desc& desc);
	
}; // </ResourceSystemFactory>

} // </wt>

#endif // </WT_RESOURCESYSTEMFACTORY_H>
