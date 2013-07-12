#ifndef WT_PACKET_H
#define WT_PACKET_H

#include "wt/stdafx.h"

#include "wt/Buffer.h"

namespace wt{

namespace net{

class Packet{
private:
	Int32 mId;

	typedef Buffer<unsigned char> ByteBuffer;

	ByteBuffer mData;
	

public:
	Packet() : mId(-1){
	}

	Packet(Int32 id, const String& data) : mId(id){
		mData.create(data.size() + 1);
		mData.put((Uint8*)data.c_str(), data.size()+1);
	}

	Packet(const Packet& other){
		*this = other;
	}

	Packet(Int32 id, const char* data, Uint32 size) : mId(id){
		mData.create(size);

		if(data){
			mData.put((Uint8*)data, size);
		}
	}

	void clear(){
		mId = -1;
		mData = ByteBuffer();
	}

	void* getPayload() const{
		return (void*)(mData.getData());
	}

	Uint32 getPayloadSize() const{
		return mData.getSize();
	}

	Int32 getId() const{
		return mId;
	}

	Packet& operator=(const Packet& other){
		mId = other.mId;
		other.mData.copy(mData);

		return *this;
	}
}; // </Packet>


}; // </net>

}; // </wt>


#endif // </WT_PACKET_H>

