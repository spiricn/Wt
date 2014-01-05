#ifndef WT_PB_H
#define WT_PB_H

#include "wt/proto/Scene.pb.h"

#include "wt/ATransformable.h"

namespace wt
{

namespace pb
{

bool conv(const Vec3& src, glm::vec3* dst);

bool conv(const glm::vec3& src, Vec3* dst);


bool conv(const Vec4& src, glm::vec4* dst);

bool conv(const glm::vec4& src, Vec4* dst);


bool conv(const Vec4& src, glm::quat* dst);

bool conv(const glm::quat& src, Vec4* dst);


bool conv(const ATransformable& src, Transform* dst);

bool conv(const Transform& src, ATransformable* dst);


} // </pb>

} // </wt>

#endif // </WT_PB_H>
