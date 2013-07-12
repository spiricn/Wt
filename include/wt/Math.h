#ifndef WT_MATH_H
#define WT_MATH_H


#include "wt/stdafx.h"


#include "wt/Exception.h"

namespace wt{

namespace math{

static const float PI = 3.14159265358979323846f;

/**
	decomposes a quaternion to axis/angle representation
	the results are stored in a vec4 where x,y,z represent the rotation axis
	and w component represents the angle in DEGREES
*/
void quatToAxisAngle(const glm::quat& quat, glm::vec4& res);

inline bool isPowerOfTwo(Uint32 number){
    while (number != 0){
        if (number == 1){
            return true;
		}

        if ((number & 1) == 1){
            // number is an odd number and not 1 - so it's not a power of two.
            return false;
		}

        number = number >> 1;
    }
    return false;
}
/**
	returns a random number in range 0.0f to 1.0f
*/
inline float random(){
	static bool isSeeded=false;
	if(!isSeeded){
		srand((unsigned int)time(NULL));
		isSeeded=true;
	}


	return (float)rand()/(float)RAND_MAX;
}

inline glm::vec3 project(const glm::vec3& vertex, float screenWidth, float screenHeight,
	const glm::mat4x4& modelViewMat, const glm::mat4x4& projectionMat){
		return glm::project(vertex, modelViewMat, projectionMat,
			glm::vec4(0, 0, screenWidth, screenHeight));
}

inline glm::vec3 unProject(float x, float y, float screenWidth, float screenHeight,
	const glm::mat4x4& modelViewMat, const glm::mat4x4& projectionMat){

		glm::vec4 viewport(0, 0, screenWidth, screenHeight);
		glm::vec3 win(x, screenHeight-y, 1.0f);

		return glm::unProject(win, modelViewMat, projectionMat, viewport);
}

inline int random(int start, int end){
	return start + (int)( (end-start)*random() );
}

template<class T>
T linearInterpolation(const T& a, const T& b, float factor){
	return a + (b-a)*factor;
}

template<class T>
T cosineInterpolation(const T& a, const T& b, float factor){
	float ft = factor * math::PI;
	float f = (1 - cosf(ft)) * 0.5f;

	return a*(1-f) + b*f;
}
 
inline float random(float start, float end){
	return start + (end-start)*random();
}

inline String randomString(Uint32 size){
	std::stringstream ss;

	for(Uint32 i=0; i<size; i++){
		ss << (char)( random()>0.5f?random('a', 'z'):random('A', 'Z'));
	}

	return ss.str();
}

void pointOnSphere(float radius, float theta, float rho, const glm::vec3& pos, glm::vec3& res);

void pointOnCircle(float radius, float theta, glm::vec2& res);

void extractTranslation(const glm::mat4& mat, glm::vec3& res);

void decomposeMatrix(const glm::mat4x4& mat, glm::quat& rotation,
	glm::vec3& translation, glm::vec3& scale);

inline float degToRad(float deg){
	return (deg * PI)/180.0f;
}

inline float radToDeg(float rad){
	return rad * 180 / PI; 
}



inline float vectorAngle(const glm::vec3& vec){
	float x = vec.x;
	float z = vec.z;
	float angle = math::radToDeg( atan(z/x ) );

	// 1st quadrant
	if(x >= 0 && z >= 0){
	}
	// 2nd quadrant
	else if(x < 0 && z >= 0){
		angle += 180;

	}
	// 3rd quadrant
	else if(x < 0 && z < 0 ){
		angle += 180;
	}
	// 4th quadrant
	else{
		angle += 360;
	}

	angle = 360-angle;

	return angle;
}

inline float vectorAngle(const glm::vec2& vec){
	return vectorAngle(glm::vec3(vec.x, 0, vec.y));
}

inline float hermite(float p1, float p2, float m1, float m2, float t){
	return (float) ((2*pow(t,3) - 3*pow(t,2) + 1) * p1 +
	(pow(t,3) - 2*pow(t,2) + t) * m1 +
	(-2*pow(t,3) + 3*pow(t,2)) * p2 +
	(pow(t,3) - pow(t,2)) * m2);
}

}; // </math>

}; // </wt>

#endif
