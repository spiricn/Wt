#ifndef WT_ARESOURCELOADER_H
#define WT_ARESOURCELOADER_H


#include "wt/stdafx.h"
#include "wt/AIOStream.h"
#include "wt/FileIOStream.h"
#include "wt/Sp.h"
#include "wt/AFileSystem.h"

#define TD_TRACE_TAG "AResourceLoader"

namespace wt
{

class AResourceProvider{
public:
	virtual Sp<AIOStream> openStream(const String& uri, AIOStream::Mode mode) = 0;
};

class FileResourceProvider{
public:
	Sp<AIOStream> openStream(const String& uri, AIOStream::Mode mode){
		return new FileIOStream(uri, mode);
	}
};

template<class T>
class AResourceLoader{
public:
	virtual void load(AIOStream* stream, T* dst) = 0;

	virtual void save(AIOStream* stream, T* src) = 0;

	virtual void load(const String& path, T* rsrc){
		AResourceManager<T>* manager = rsrc->getManager();
		WT_ASSERT(manager, "Resource not managed");

		Sp<AIOStream> stream = manager->getResourceSystem()->getFileSystem()->open(path, AIOStream::eMODE_READ);
		WT_ASSERT(stream->isReadable(), "Error openning file stream \"%s\"", path.c_str());
		load(stream, rsrc);
	}

	virtual void save(const String& path, T* rsrc){
		AResourceManager<T>* manager = rsrc->getManager();
		WT_ASSERT(manager, "Resource not managed");

		Sp<AIOStream> stream = manager->getResourceSystem()->getFileSystem()->open(path, AIOStream::eMODE_WRITE);
		WT_ASSERT(stream->isReadable(), "Error openning file stream \"%s\"", path.c_str());
		save(stream, rsrc);
	}

	virtual void create(T* resource){
	}

}; // <AResourceLoader>

} // </wt>

#define TD_TRACE_TAG ""

#endif // </WT_ARESOURCELOADER_H>
