#include "wt/stdafx.h"

#include "wt/PxWt.h"

using namespace physx;

namespace wt
{

void pxConvert(const glm::vec3& src, physx::PxVec3& dst){
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

void pxConvert(const physx::PxVec3& src, glm::vec3& dst){
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

void pxConvert(const physx::PxExtendedVec3& src, glm::vec3& dst){
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}


void pxConvert(const glm::vec3& src, physx::PxExtendedVec3& dst){
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

void pxConvert(const PxTransform& src, ATransformable& dst){
	dst.setTranslation( glm::vec3(src.p.x, src.p.y, src.p.z) );
	dst.setRotation( glm::vec3(src.q.x, src.q.y, src.q.z), src.q.w );
}

void pxConvert(const ATransformable& src, PxTransform& dst){
	glm::vec3 pos;
	glm::quat rot;
	src.getTranslation(pos);
	src.getRotation(rot);

	dst.p.x = pos.x;
	dst.p.y = pos.y;
	dst.p.z = pos.z;

	dst.q.x = rot.x;
	dst.q.y = rot.y;
	dst.q.z = rot.z;
	dst.q.w = rot.w;
}

void pxConvert(const physx::PxTransform& src, glm::mat4& dst){
	glm::vec3 pos;
	glm::quat rot;

	pxConvert(src.p, pos);
	pxConvert(src.q, rot);

	dst = glm::translate(pos) * glm::mat4_cast(rot);
}

void pxConvert(const glm::mat4& src, physx::PxTransform& dst){
	glm::vec3 pos, scale;
	glm::quat rot;

	math::decomposeMatrix(src, rot, pos, scale);

	pxConvert(pos, dst.p);
	pxConvert(rot, dst.q);
}

void pxConvert(const physx::PxQuat& src, glm::quat& dst){
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
	dst.w = src.w;
}

void pxConvert(const glm::quat& src, physx::PxQuat& dst){
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
	dst.w = src.w;
}

} // </wt>
