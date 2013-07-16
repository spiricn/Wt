#pragma once
#ifndef WT_AIOSTREAM_H
#define WT_AIOSTREAM_H

#include "wt/stdafx.h"

namespace wt{

class AIOStream{
public:
	enum Mode{
		eMODE_READ,
		eMODE_WRITE,
		eMODE_NONE
	}; // </Mode>

	enum SeekOrigin{
		eSEEK_BEGGINING,
		eSEEK_END,
		eSEEK_CURRENT
	}; // </SeekOrigin>

private:
	Mode mMode;

protected:
	void setMode(Mode mode);

public:
	AIOStream();

	virtual ~AIOStream();

	Mode getMode() const;

	virtual int16_t get();

	virtual int64_t put(char c);

	virtual int64_t puts(const String& src);

	virtual int64_t gets(String& dst);

	/**
	 * Helper seek function which takes an absolute position rather than using origin/offset combo.
	 *
	 * @param position	Absolute position in the stream (from the beggining)
	 * @return Position sought to or -1 on error
	 */
	virtual int64_t seek(int64_t position);

	/**
	 * Formatted write.
	 * Analog to fprintf.
	 */
	virtual int64_t print(const char* fmt, ...);

	virtual void flush();

	virtual bool isReadable();

	virtual bool isWritable();

	int64_t getLine(String& dst, char delim, uint32_t limit=0xFFFFFFFF);

	/**
	 * Read @size bytes into the destination buffer.
	 *
	 * @param dst Destination buffer to which the bytes are going to be written.
	 * @param size Number of bytes to read.
	 * @return Number of bytes read, or -1 on error.
	 */
	virtual int64_t read(void* dst, int64_t size) = 0;

	/**
	 * Writes @size bytes into the stream.
	 *
	 * @param src Source buffer from which the bytes are going to be read.
	 * @param size Number of bytes to write.
	 * @return Number of bytes written, or -1 on error.
	 */
	virtual int64_t write(const void* src, int64_t size) = 0;

	/**
	 * Seeks the stream.
	 *
	 * @param origin Starting point of the seek.
	 * @param offset Offset of the seak.
	 * @return Absolute location to which we sought, or -1 on error.
	 */
	virtual int64_t seek(SeekOrigin origin, int64_t offset) = 0;
	
	virtual int64_t tell() = 0;

	virtual int64_t getSize() = 0;

	virtual bool isOpen() = 0;

	virtual bool isSeekable() = 0;

}; // </AInputStream>



}; // </wt>


#endif // </WT_AIOSTREAM_H;