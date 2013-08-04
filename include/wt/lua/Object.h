#ifndef WT_LUAOBJECT_H
#define WT_LUAOBJECT_H

#include <LuaPlus.h>

namespace wt{

namespace lua{

using namespace LuaPlus;

struct MetaInfo{
	LuaObject* meta;

	MetaInfo() : meta(NULL){
	}
}; // </MetaInfo>


class State;

template<class T>
class Object{

friend class State;

protected:
	virtual void generateMetaTable() = 0;
	
	template<typename Func>
	void expose(const char* name, Func funcPtr){
		getMetaInfo()->meta->RegisterObjectDirect(name, (T*)0, funcPtr);
	}

	static MetaInfo* getMetaInfo(){
		if(sMetaInfo == NULL){
			sMetaInfo = new MetaInfo;
		}

		return sMetaInfo;
	}

	virtual ~Object(){
	}

private:
	static MetaInfo* sMetaInfo;

}; // </Object>

template<class T>
MetaInfo* Object<T>::sMetaInfo = NULL;

}; // </lua>

}; // </wt>

#endif // </WT_LUAOBJECT_H>