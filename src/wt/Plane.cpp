// Plane.cpp
//
//////////////////////////////////////////////////////////////////////

#include "wt/stdafx.h"
#include "wt/Plane.h"
#include <stdio.h>

namespace wt
{

Plane::Plane( glm::vec3 &v1,  glm::vec3 &v2,  glm::vec3 &v3) {

	set3Points(v1,v2,v3);
}


Plane::Plane() {}

Plane::~Plane() {}


void Plane::set3Points( glm::vec3 &v1,  glm::vec3 &v2,  glm::vec3 &v3) {


	glm::vec3 aux1, aux2;

	aux1 = v1 - v2;
	aux2 = v3 - v2;

	normal = aux2 * aux1;

	normal = glm::normalize(normal);
	point = v2;
	d = -(glm::dot(normal, point));
}



float Plane::distance(glm::vec3 &p) {
	return (d + glm::dot(normal, p));
}

} // </wt>