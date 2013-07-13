#pragma once
#ifndef WT_ZIPINPUSTREAM_H
#define WT_ZIPINPUSTREAM_H

#include "wt/stdafx.h"

#include "wt/AIOStream.h"

#include <physfs.h>

namespace wt{

class ZipInputStream : public AIOStream{
public:
	ZipInputStream(PHYSFS_file* file);

	~ZipInputStream();

	int64_t read(void* dst, int64_t size);

	int64_t write(const void* src, int64_t size);

	int64_t seek(SeekOrigin origin, int64_t offset);

	int64_t tell();

	int64_t getSize();

	bool isOpen();

	bool isReadable();

	bool isWritable();

	bool isSeekable();

	void flush();

private:
	PHYSFS_file* mFile;

}; // </ZipInputStream>


}; // </wt>

#endif // </WT_ZIPINPUSTREAM_H>