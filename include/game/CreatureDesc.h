#ifndef WT_CREATUREDESC_H
#define WT_CREATUREDESC_H

#include "wt/Lua.h"
#include "wt/Singleton.h"
#include "wt/AResourceManager.h"

namespace wt{

class CreatureDesc : public AResource<CreatureDesc>{
public:
	struct SoundDesc{
		String attack;
		String aggro;
		String wound;
		String walk;
		String death;
	} soundDesc;

	struct ModelDesc{
		String model;
		String skin;
	} modelDesc;

	struct StatDesc{
		float maxHitPoints;
		float damage;
		float critChance;
		float critModifier;
		float attackSpeed;
		float moveSpeed;
		float aggroRange;
		float attackRange;
		float defense;
	} statDesc;

	void serialize(LuaObject& dst){
		// sounds
		dst.Set("snd_attack", soundDesc.attack.c_str());
		dst.Set("snd_aggro", soundDesc.aggro.c_str());
		dst.Set("snd_wound", soundDesc.wound.c_str());
		dst.Set("snd_walk", soundDesc.walk.c_str());
		dst.Set("snd_death", soundDesc.death.c_str());

		// model
		dst.Set("mdl_model", modelDesc.model.c_str());
		dst.Set("mdl_skin", modelDesc.skin.c_str());

		// stats
		dst.Set("st_max_hp", statDesc.maxHitPoints);
		dst.Set("st_damage", statDesc.damage);
		dst.Set("st_critChance", statDesc.critChance);
		dst.Set("st_critMod", statDesc.critModifier);
		dst.Set("st_attackSpeed", statDesc.attackSpeed);
		dst.Set("st_moveSpeed", statDesc.moveSpeed);
		dst.Set("st_aggroRange", statDesc.aggroRange);
		dst.Set("st_attackRange", statDesc.attackRange);
		dst.Set("st_defense", statDesc.defense);
	}

	void deserialize(LuaObject& src){
		modelDesc.model = src.Get("mdl_model").ToString();
		modelDesc.skin = src.Get("mdl_skin").ToString();

		soundDesc.attack = src.Get("snd_attack").ToString();
		soundDesc.aggro = src.Get("snd_aggro").ToString();
		soundDesc.wound = src.Get("snd_wound").ToString();
		soundDesc.walk = src.Get("snd_walk").ToString();
		soundDesc.death = src.Get("snd_death").ToString();

		statDesc.maxHitPoints = src.Get("st_max_hp").ToNumber();
		statDesc.damage = src.Get("st_damage").ToNumber();
		statDesc.critChance = src.Get("st_critChance").ToNumber();
		statDesc.critModifier = src.Get("st_critMod").ToNumber();
		statDesc.attackSpeed = src.Get("st_attackSpeed").ToNumber();
		statDesc.moveSpeed = src.Get("st_moveSpeed").ToNumber();
		statDesc.aggroRange = src.Get("st_aggroRange").ToNumber();
		statDesc.attackRange = src.Get("st_attackRange").ToNumber();
		statDesc.defense = src.Get("st_defense").ToNumber();
	}

	CreatureDesc(ResourceHandle handle=0, const String& name="") : AResource(handle, name){
	}

}; // </CreatureDesc>


class CreatureDescManager : public AResourceManager<CreatureDesc>, public Singleton<CreatureDescManager>{
public:

}; // </CreatureDescManager >

}; // </wt>

#endif // </WT_CREATUREDESC_H>