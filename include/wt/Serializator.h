/**
 * @file Serializator.h
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */
 
#ifndef WT_SERIALIZATOR_H
#define WT_SERIALIZATOR_H

#include "wt/stdafx.h"
#include "wt/Singleton.h"
#include "wt/Field.h"

#define FIELD(val) do{ addField(#val, &val); }while(0);

#define SER_FIELD(val) do{ addField(#val, dynamic_cast<ISerializator*>(&val)); }while(0);

#define NAMED_FIELD(name, val) do{ addField(name, val); }while(0);

#define NAMED_SER_FIELD(name, val) do{ addField(name, dynamic_cast<ISerializator*>(&val)); }while(0);

#define BEG_FIELDS void __createSerializatorFields(void){

#define END_FIELDS }

namespace wt
{

template<class T>
class ASerializator : public Singleton< ASerializator<T> >, public ISerializator{
public:
	ASerializator() : mImpl(NULL){
	}

	virtual void __createSerializatorFields(){
	}

	void deserialize(lua::State* state, LuaObject table){
		ASerializator& s = getSingleton();

		if(!s.mImpl){
			init();
		}

		for(FieldList::iterator iter=s.mImpl->fields.begin(); iter!=s.mImpl->fields.end(); iter++){
			(*iter)->deserialize(state, dynamic_cast<T*>(this), table);
		}
	}

	void serialize(lua::State* state, LuaObject table){
		ASerializator& s = getSingleton();

		if(!s.mImpl){
			init();
		}

		for(FieldList::iterator iter=s.mImpl->fields.begin(); iter!=s.mImpl->fields.end(); iter++){
			(*iter)->serialize(state, dynamic_cast<T*>(this), table);
		}
	}

	template<class ValueType>
	void addField(const char* name, ValueType* value){
		ASerializator& s = getSingleton();

		uint32_t offset = reinterpret_cast<uint8_t*>(value) - reinterpret_cast<uint8_t*>(dynamic_cast<T*>(this));

		s.mImpl->fields.insert( new Field<ValueType, T>(name, offset) );
	}

private:
	void init(){
		ASerializator& s = getSingleton();

		s.mImpl = new Impl();

		__createSerializatorFields();

		s.mImpl->fieldsCreated = true;
	}

private:
	typedef std::set<IField*> FieldList;

	struct Impl{
		Impl() : fieldsCreated(false){
		}

		bool fieldsCreated;
		FieldList fields;
	}; // </Impl>

private:
	Impl* mImpl;

}; // </ASerializator>

} // </wt>

#endif // </WT_SERIALIZATOR_H>
