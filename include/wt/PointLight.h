#ifndef WT_POINTLIGHT_H
#define WT_POINTLIGHT_H

#include "wt/ALight.h"
#include "wt/ASceneActor.h"

namespace wt
{

class PointLight : public ALight, public ASceneActor, public ATransformable{
friend class Scene;

public:
	struct Desc : public ALight::Desc{
		glm::vec3 position;

		struct{
			float constant;
			float linear;
			float quadratic;
		} attenuation;

		float calculateBoundingSphere() const;

		Desc();
	}; // </Desc>

	void serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst, void* opaque=NULL);

	void deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src, void* opaque=NULL);

	const Desc& getDesc() const;

	void setDesc(const Desc& desc) const;

	ATransformable* getController();

	const ATransformable* getTransformable() const;

	virtual void setTranslation(const glm::vec3& translation);

	virtual void setRotation(const glm::quat& rotation);

	virtual void setScale(const glm::vec3& scale);

	virtual void getScale(glm::vec3& result) const;

	virtual void getTranslation(glm::vec3& result) const;

	virtual void getRotation(glm::quat& result) const;

protected:
	PointLight(Scene* parent, uint32_t id, const String& name);

	Desc& getDesc();

private:
	PointLight(const PointLight&);

	Desc mDesc;
}; // </PointLight>

} // </wt>

#endif // </WT_POINTLIGHT_H>