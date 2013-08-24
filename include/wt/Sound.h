#ifndef WT_SOUND_H
#define WT_SOUND_H

#include "wt/ASceneActor.h"
#include "wt/ASoundSystem.h"

namespace wt
{

class Sound : public ASceneActor, public ATransformable{
public:
	Sound(Scene* parent, uint32_t id, const String& name="");

	ATransformable* getTransformable();

	SoundPtr getSound() const;

	void update(float dt);

	void setSound(SoundPtr sound);

	virtual void setTranslation(const glm::vec3& translation);

	virtual void setRotation(const glm::quat& rotation);

	virtual void setScale(const glm::vec3& scale);

	virtual void getScale(glm::vec3& result) const;

	virtual void getTranslation(glm::vec3& result) const;

	virtual void getRotation(glm::quat& result) const;

	void serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst, void* opaque=NULL);

	void deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src, void* opaque=NULL);

private:
	SoundPtr mSound;
}; // </Sound>

} // </wt>

#endif // </WT_SOUND_H>