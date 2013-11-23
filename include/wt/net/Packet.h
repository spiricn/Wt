#ifndef WT_PACKET_H
#define WT_PACKET_H

#include "wt/stdafx.h"

#include "wt/Buffer.h"

namespace wt
{

namespace net
{

class Packet{
public:
	Packet();

	Packet(const String& data);

	Packet(const void* data, uint32_t size);

	Packet(const Packet& other);

	void clear();

	ByteBuffer& getPayload();

	const ByteBuffer& getPayload() const;

	template<class T>
	void write(const T& val);

	void write(const void* data, uint32_t size);

	template<>
	void write<String>(const String& val);

	template<class T>
	void read(T* val);

	void read(void* data, uint32_t size);

	template<class T>
	T read();

	template<>
	void read<String>(String* val);

private:
	ByteBuffer mData;

}; // </Packet>


template<class T>
void Packet::write(const T& val){
	mData.put( reinterpret_cast<const uint8_t*>(&val), sizeof(T) );
}

template<class T>
void Packet::read(T* val){
	mData.get( reinterpret_cast<uint8_t*>(val), sizeof(T) );
}

template<>
void Packet::write<String>(const String& val){
	mData.put( reinterpret_cast<const uint8_t*>( val.c_str() ), val.size() + 1);
}

template<>
void Packet::read<String>(String* val){
	// TODO probably not safe
	*val = String( reinterpret_cast<char*>( mData.getData() + mData.tellg()  ) );
	mData.seekg( mData.tellg() + val->size() + 1 );
}

template<class T>
T Packet::read(){
	T res;
	read(&res);

	return res;
}

} // </net>

} // </wt>


#endif // </WT_PACKET_H>

