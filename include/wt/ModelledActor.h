#ifndef WT_MODELLEDACTOR_H
#define WT_MODELLEDACTOR_H

#include "wt/ASceneActor.h"
#include "wt/ASerializable.h"
#include "wt/SkeletalAnimationPlayer.h"
#include "wt/FileIOStream.h"
#include "wt/PhysicsActor.h"
#include "wt/lua/Object.h"

namespace wt{


class ModelledActor : public ASceneActor, public lua::Object<ModelledActor>{
public:
	ModelledActor(Scene* parent, uint32_t id, const String& name="");

	bool hasAnimation() const;

	Model* getModel() const;

	void setModel(Model* model, const String& skin);

	SkeletalAnimationPlayer* getAnimationPlayer() const;

	Model::GeometrySkin* getSkin() const;

	void setSkin(const String& name);

	void setSkin(Model::GeometrySkin* skin);

	bool validAttachPoint(const String& pointId) const;

	bool getAttachPointTransform(const String& pointId, glm::mat4& res) const;

	void update(float dt);

	void serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst, void* opaque=NULL);

	void deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src, void* opaque);

	ATransformable* getTransformable();

	struct DeserializationData{
		PhysicsActor::Desc pxDesc;
		bool phyiscs;
	};


	// Lua bindings
	void generateMetaTable();

	glm::vec3 lua_getPosition();

	lua::LuaObject lua_getTransform();

	void lua_playAnimation(const char* name, bool loop);

	void lua_move(const glm::vec3& offset);

private:
	Model* mModel;
	Model::GeometrySkin* mSkin;
	SkeletalAnimationPlayer* mAnimationPlayer;
	math::Transform mTransform;

}; // </ModeledActor>

}; // </wt>

#endif // </WT_MODELLEDACTOR_H>
