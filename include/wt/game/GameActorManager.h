#ifndef WT_GAMEACTORMANAGER_H
#define WT_GAMEACTORMANAGER_H

#include "wt/game/AGameActor.h"
#include "wt/AProcess.h"

namespace wt{


class EntityManager : public AProcess, public Lua::ASerializable{
public:
	typedef AGameActor* EntityPtr;

	typedef std::map<AGameActor::Handle, EntityPtr> EntityMap;

private:
	
	
	EntityMap mEntities;

public:

	void clear(){
		for(EntityMap::iterator i=mEntities.begin(); i!=mEntities.end(); i++){
			delete i->second;
		}

		mEntities.clear();
	}

	void onProcUpdate(float dt){
		for(EntityMap::iterator i=mEntities.begin(); i!=mEntities.end(); i++){
			if(!i->second->isAlive()){
				i = mEntities.erase(i);
				//delete i->second;
			}
			else{
				i->second->onUpdate(dt);
			}

			if(i==mEntities.end()){
				break;
			}
		}
	}

	EntityMap& getActors(){
		return mEntities;
	}

	
	AGameActor* getEntity(const AGameActor::Handle& handle){
		return 0;
	}

	void addEntity(const String& name, EntityPtr entity){
		mEntities[name] = entity;
	}

	bool contains(const String& name) const{
		return findByName(name) != NULL;
	}

	void deleteEntity(const String& name){
		deleteEntity( mEntities[name] );
	}

	void deleteEntity(EntityPtr entity){
		mEntities.erase(entity->getName());
	}

	EntityPtr findByName(const String& name) const{
		EntityMap::const_iterator i = mEntities.find(name);
		return i==mEntities.end() ? NULL : i->second;
	}

	void serialize(LuaObject& dst){
		for(EntityMap::iterator i=mEntities.begin(); i!=mEntities.end(); i++){
			LuaObject table;
			LUA_NEW_TABLE(table);

			dst.Set(i->first.c_str(), table);
			i->second->serialize(table);
		}
	}

	void deserialize(const LuaPlus::LuaObject& src){
	}

};

}; // </wt>

#endif // </WT_GAMEACTORMANAGER_H>