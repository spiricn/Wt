#ifndef WT_FILESYSTEMFACTORY_H
#define WT_FILESYSTEMFACTORY_H

#include "wt/AFileSystem.h"

namespace wt
{

class FileSystemFactory{
public:
	static AFileSystem* create(const AFileSystem::Desc&);

}; // </FileSystemFactory>

} // </wt>

#endif // </WT_FILESYSTEMFACTORY_H>
