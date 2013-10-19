#ifndef WT_PXWT_H
#define WT_PXWT_H

#include "wt/Transform.h"

namespace wt
{

// Vec3
void pxConvert(const glm::vec3& src, physx::PxVec3& dst);

void pxConvert(const physx::PxVec3& src, glm::vec3& dst);

// ExtendedVec3
void pxConvert(const physx::PxExtendedVec3& src, glm::vec3& dst);

void pxConvert(const glm::vec3& src, physx::PxExtendedVec3& dst);

// ATransformable
void pxConvert(const physx::PxTransform& src, ATransformable& dst);

void pxConvert(const ATransformable& src, physx::PxTransform& dst);

// mat4
void pxConvert(const physx::PxTransform& src, glm::mat4& dst);

void pxConvert(const glm::mat4& src, physx::PxTransform& dst);

// quat
void pxConvert(const physx::PxQuat& src, glm::quat& dst);

void pxConvert(const glm::quat& src, physx::PxQuat& dst);

} // </wt>

#endif // </WT_PXWT_H>
