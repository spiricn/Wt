#include "wt/stdafx.h"

#include "wt/math.h"

namespace wt{

namespace math{

void pointOnSphere(float radius, float theta, float rho, const glm::vec3& pos, glm::vec3& res){
	res.x = pos.x + radius * cosf(theta) * sinf(rho);
	res.z = pos.z + radius * sinf(theta) * sinf(rho);
	res.y = pos.y + radius * cosf(rho);
}

/**
	theta - circle angle in radians (ranges from 0 to 2PI)
*/
void pointOnCircle(float radius, float theta, glm::vec2& res){
	res.x = radius * cosf(theta);
	res.y = radius * sinf(theta);
}

void extractTranslation(const glm::mat4& mat, glm::vec3& res){
	res.x = mat[3][0];
	res.y = mat[3][1];
	res.z = mat[3][2];
}

void decomposeMatrix(const glm::mat4x4& mat, glm::quat& rotation,
	glm::vec3& position, glm::vec3& scaling){
	
	// extract translation
	position.x = mat[3][0];
	position.y = mat[3][1];
	position.z = mat[3][2];

	// extract the rows of the matrix
	glm::vec3 vRows[3] = {
		glm::vec3(mat[0][0], mat[0][1], mat[0][2]),
		glm::vec3(mat[1][0], mat[1][1], mat[1][2]),
		glm::vec3(mat[2][0], mat[2][1], mat[2][2])
	};

	// extract the scaling factors
	scaling.x = glm::length(vRows[0]);
	scaling.y = glm::length(vRows[1]);
	scaling.z = glm::length(vRows[2]);

	// and the sign of the scaling
	if (glm::determinant(mat) < 0) {
		scaling.x = -scaling.x;
		scaling.y = -scaling.y;
		scaling.z = -scaling.z;
	}

	// and remove all scaling from the matrix
	if(scaling.x)
	{
		vRows[0] /= scaling.x;
	}
	if(scaling.y)
	{
		vRows[1] /= scaling.y;
	}
	if(scaling.z)
	{
		vRows[2] /= scaling.z;
	}

	// build a 3x3 rotation matrix
	glm::mat3 m(
		vRows[0].x, vRows[1].x, vRows[2].x,
		vRows[0].y, vRows[1].y, vRows[2].y,
		vRows[0].z, vRows[1].z, vRows[2].z);
	m = glm::transpose(m);

	// and generate the rotation quaternion from it
	rotation = glm::quat_cast(m);
}

void quatToAxisAngle(const glm::quat& quat, glm::vec4& res){
	res.w = math::radToDeg( 2.0f*acos(quat.w) );

	float wSquared = quat.w*quat.w;

	res.x = quat.x / sqrt(1-wSquared);
	res.y = quat.y / sqrt(1-wSquared);
	res.z = quat.z / sqrt(1-wSquared);
}

}; // </math>

}; // </wt>
