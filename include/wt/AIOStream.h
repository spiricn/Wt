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
	void setMode(Mode mode){
		mMode = mode;
	}

public:
	AIOStream() : mMode(eMODE_NONE){
	}

	virtual ~AIOStream(){
	}

	Mode getMode() const{
		return mMode;
	}

	/**
	 * Helper seek function which takes an absolute position rather than using origin/offset combo.
	 *
	 * @param position	Absolute position in the stream (from the beggining)
	 * @return Position sought to or -1 on error
	 */
	virtual int64_t seek(int64_t position){
		return seek(eSEEK_BEGGINING, position);
	}

	/**
	 * Read @size bytes into the destination buffer.
	 *
	 * @param dst Destination buffer to which the bytes are going to be written.
	 * @param size Number of bytes to read.
	 * @return Number of bytes read, or -1 on error.
	 */
	virtual int64_t read(uint8_t* dst, int64_t size) = 0;

	/**
	 * Writes @size bytes into the stream.
	 *
	 * @param src Source buffer from which the bytes are going to be read.
	 * @param size Number of bytes to write.
	 * @return Number of bytes written, or -1 on error.
	 */
	virtual int64_t write(const uint8_t* src, int64_t size) = 0;

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

	virtual bool isReadable() = 0;

	virtual bool isWritable() = 0;

	virtual bool isSeekable() = 0;

	virtual void flush(){
	}
}; // </AInputStream>



}; // </wt>

#endif // </WT_AIOSTREAM_H;