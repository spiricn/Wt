#ifndef WT_SOUND_H
#define WT_SOUND_H

#include "wt/ASceneActor.h"
#include "wt/ASoundSystem.h"

namespace wt
{

class Sound : public ASceneActor, public ATransformable{
public:
	Sound(Scene* parent, uint32_t id, const String& name="");

	ATransformable* getTransformable() const;

	SoundPtr getSound() const;

	void update(float dt);

	void setSound(SoundPtr sound);

	void setTranslation(const glm::vec3& translation);

	void setRotation(const glm::quat& rotation);

	void setScale(const glm::vec3& scale);

	void getScale(glm::vec3& result) const;

	void getTranslation(glm::vec3& result) const;

	void getRotation(glm::quat& result) const;

	void serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst, void* opaque=NULL);

	void deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src, void* opaque=NULL);

private:
	SoundPtr mSound;
}; // </Sound>

} // </wt>

#endif // </WT_SOUND_H>