#ifndef WT_ARESOURCEGROUP_H
#define WT_ARESOURCEGROUP_H


#include "wt/stdafx.h"

#include "wt/AResource.h"
#include "wt/Exception.h"
#include "wt/Utils.h"
#include "wt/ASerializable.h"
#include "wt/AResourceSystem.h"
#include "wt/lua/State.h"

namespace wt{

template<typename T>
class AResourceGroup;

template<typename T>
class AResourceManager;

template<class T>
class AResourceAllocator{
public:
	virtual T* allocate(AResourceGroup<T>* parent, const String& resourceName)=0;
	virtual void free(T* resource)=0;
};

template<class T>
class AResourceGroup : public lua::ASerializable{
	friend class AResourceManager<T>;
public:
	typedef std::map<String, AResourceGroup<T>*> GroupMap;
	typedef std::map<ResourceHandle, T*> ResourceMap;
private:
	String mName;
	GroupMap mChildren;
	AResourceAllocator<T>* mAllocator;
	ResourceMap mResources;
	AResourceGroup<T>* mParent;
	AResourceSystem* mResourceSystem;

protected:
	// Only resource manager is allowed access
	void setResourceAllocator(AResourceAllocator<T>* allocator){
		mAllocator = allocator;
	}

	void setName(const String& name){
		mName = name;
	}

	void setResourceSystem(AResourceSystem* system){
		mResourceSystem = system;
	}

	AResourceGroup(AResourceAllocator<T>* allocator,
		AResourceGroup* parent, AResourceSystem* resourceSystem, const String& name) : mAllocator(allocator),
		mName(name), mParent(parent), mResourceSystem(resourceSystem){
	}

	
	AResourceGroup() : mAllocator(NULL),
		mName(""), mParent(NULL), mResourceSystem(NULL){
	}

	// prevent copy constructor
	AResourceGroup(const AResourceGroup& other){ assert(0&&"Copy constructor disabled"); }

public:

	AResourceSystem* getResourceSystem() const{
		return mResourceSystem;
	}

	virtual ~AResourceGroup(){
	}

	GroupMap& getChildren(){
		return mChildren;
	}

	ResourceMap& getResources(){
		return mResources;
	}

	bool contains(const String& name){
		return find(name, true)!=NULL;
	}

	const String& getName() const{
		return mName;
	}

	/** create new resource */
	virtual T* create(const String& name){
		// prohibit resources with a same name
		if(contains(name)){
			WT_THROW("AResource named \"%s\" already exists", name.c_str());
		}

		// allocate resource & add store it
		T* rsrc = mAllocator->allocate(this, name);
		
		mResources[rsrc->getHandle()] = rsrc;

		return rsrc;
	}

	 
	/** find an existing resource by name */
	T* find(const String& name, bool recursive=true){
		for(ResourceMap::iterator i=mResources.begin(); i!=mResources.end(); i++){
			if(i->second->getName().compare(name)==0){
				return i->second;
			}
		}

		if(recursive){
			for(GroupMap::iterator i=mChildren.begin(); i!=mChildren.end(); i++){
				T* res = i->second->find(name, recursive);
				if(res != NULL){
					return res;
				}
			}
		}


		//LOGW("ResourceGroup", "Unable to find resource \"%s\" in group \"%s\"", name.c_str(), mName.c_str());
		
		return NULL;
	}

	AResourceGroup<T>* createGroup(const String& name){
		AResourceGroup<T>* res = new AResourceGroup<T>(mAllocator, this, mResourceSystem, name);

		mChildren[name] = res;

		return res;
	}

	AResourceGroup<T>* findGroup(const String& name, bool recursive=true){
		for(GroupMap::iterator i=mChildren.begin(); i!=mChildren.end(); i++){
			if(i->first.compare(name)==0){
				return i->second;
			}
		}

		for(GroupMap::iterator i=mChildren.begin(); i!=mChildren.end(); i++){
			AResourceGroup<T>* res = i->second->findGroup(name, recursive);
			if(res != NULL){
				return res;
			}
		}

		return NULL;
	}

	virtual void destroy(){
		mResources.clear();

		for(GroupMap::iterator i=mChildren.begin(); i!=mChildren.end(); i++){
			i->second->destroy();
			delete i->second;
		}

		mChildren.clear();
	}


	AResourceGroup* getParent(){
		return mParent;
	}


	String getAbsPath(){
		String res = mName;

		AResourceGroup* group = mParent;
		do{
			if(group == NULL){
				break;
			}

			std::stringstream ss;

			ss << group->mName << "/" << res;
			res = ss.str();

			group = group->getParent();
		}while(1);

		return res;
	}

	AResourceGroup* getGroupFromPath(const String& path){
		size_t pos = 0;
		String rest = path;
		AResourceGroup* group = this;

		do{
			pos = rest.find("/");
			
			String base = rest.substr(0, pos);
			if(pos != String::npos){
				String r = rest.substr(pos+1);
				rest = r;
			}
			

			if(base.compare(mName)==0){
				continue;
			}

			if((group = group->findGroup(base))==NULL){
				return NULL;
			}
			
		}while(pos != String::npos);

		return group;
	}

	T* getFromPath(const String& path){
		size_t pos = path.rfind("/");
		if(pos == String::npos){
			return find(path, false);
		}
		else{
			AResourceGroup* group = getGroupFromPath(path.substr(0, pos));
			if(group != NULL){
				return group->find(path.substr(pos+1));
			}
			else{
				return NULL;
			}
		}
	}

	virtual void serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const{
		dst.Set("type", "GROUP");
		

		LuaPlus::LuaObject content = mResourceSystem->getLuastate()->newTable();
		
		dst.Set("content", content);

		// Serialize child groups
		for(GroupMap::const_iterator i=mChildren.cbegin(); i!=mChildren.cend(); i++){
			// Allocate new table for this group
			lua::LuaObject groupTable = luaState->newTable();

			// Serialize it and append it to this group's table
			i->second->serialize(luaState, groupTable);
			content.Set(i->first.c_str(),
				groupTable);
		}

		// Sserialize child resources
		for(ResourceMap::const_iterator i=mResources.cbegin(); i!=mResources.cend(); i++){
			// Allocate new table for this resource
			lua::LuaObject rsrcTable = luaState->newTable();

			// Serialize it and append it to this group's table
			i->second->serialize(luaState, rsrcTable);
			content.Set(i->second->getName().c_str(),
				rsrcTable);
		}

	}

	virtual void deserialize(lua::State* luaState, const LuaPlus::LuaObject& table){
		if(!table.IsTable()){
			WT_THROW("Resoruce group deserialization failed [%s], Lua object not a table", mName.c_str());
		}

		LuaPlus::LuaObject content = table.Get("content");
		if(!content.IsTable()){
			WT_THROW("Deserialization failed, \"content\" not a table");
		}

		// iterate through the table
		for (LuaPlus::LuaTableIterator it( content ); it; it.Next()){
			// get object name & type
			if(!it.GetKey().IsString() || !it.GetValue().IsTable() || !it.GetValue().Get("type").IsString()){
				WT_THROW("Invalid resource table");
			}

			const char* name = it.GetKey().ToString();
			const char* type = it.GetValue().Get("type").ToString();
		
			// create & deserialize a child group or resource
			if(strcmp("GROUP", type)==0){
				createGroup(name)->deserialize(luaState, it.GetValue() );
			}
			else if(strcmp("RESOURCE", type)==0){
				T* rsrc = create(name);
				rsrc->deserialize(luaState, it.GetValue() );
			}
		}
	}

	String getResourcePath(const String& name){
		std::stringstream ss;
		ss << getAbsPath() << '/' << name;
		return ss.str();
	}

	void deleteResource(const String& name){
		T* rsrc = this->find(name, false);

		WT_ASSERT(rsrc != NULL, "Unexisting resource \"%s\"", name.c_str());

		mResources.erase(rsrc->getHandle());

		mAllocator->free(rsrc);
	}

	void deleteChild(const String& name){
		AResourceGroup<T>* child = this->findGroup(name, false);

		WT_ASSERT(child!=NULL, "Unexisting child group \"%s\"", name.c_str());

		mChildren.erase(child->getName());
		child->destroy();
		delete child;
	}
};



}; // </wt>

#endif
