#include "wt/stdafx.h"

#include "wt/Pb.h"

namespace wt
{

namespace pb
{

bool conv(const Vec3& src, glm::vec3* dst){
	dst->x = src.x();
	dst->y = src.y();
	dst->z = src.z();

	return true;
}

bool conv(const glm::vec3& src, Vec3* dst){
	dst->set_x(src.x);
	dst->set_y(src.y);
	dst->set_z(src.z);

	return true;
}

bool conv(const Vec4& src, glm::vec4* dst){
	dst->x = src.x();
	dst->y = src.y();
	dst->z = src.z();
	dst->w = src.w();

	return true;
}

bool conv(const glm::vec4& src, Vec4* dst){
	dst->set_x(src.x);
	dst->set_y(src.y);
	dst->set_z(src.z);
	dst->set_w(src.w);

	return true;
}

bool conv(const Vec4& src, glm::quat* dst){
	dst->x = src.x();
	dst->y = src.y();
	dst->z = src.z();
	dst->w = src.w();

	return true;
}

bool conv(const glm::quat& src, Vec4* dst){
	dst->set_x(src.x);
	dst->set_y(src.y);
	dst->set_z(src.z);
	dst->set_w(src.w);

	return true;
}

bool conv(const ATransformable& src, Transform* dst){
	glm::vec3 translation, scale;
	glm::quat rotation;

	src.getTranslation(translation);
	src.getScale(scale);
	src.getRotation(rotation);

	conv(translation, dst->mutable_translation());
	conv(scale, dst->mutable_scale());
	conv(rotation, dst->mutable_rotation());

	return true;
}

bool conv(const Transform& src, ATransformable* dst){
	glm::vec3 translation, scale;
	glm::quat rotation;

	conv(src.translation(), &translation);
	conv(src.scale(), &scale);
	conv(src.rotation(), &rotation);

	dst->setTranslation(translation);
	dst->setScale(scale);
	dst->setRotation(rotation);

	return true;
}

} // </pb>

} // </wt>
