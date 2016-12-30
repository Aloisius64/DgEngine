////////////////////////////////////////////////////////////////////////////////
// Filename: Plane.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Plane.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


const float EPSILON = 0.0001f;

Plane::Plane(void){
	N = glm::vec3(0, 1, 0);
	d = 0;
}

Plane::~Plane(void){}

Plane::Plane(const glm::vec3& normal, const glm::vec3& p) {
	N = normal;
	d = -glm::dot(N, p);
}

Plane Plane::FromPoints(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
	Plane temp;
	glm::vec3 e1 = v2 - v1;
	glm::vec3 e2 = v3 - v1;
	temp.N = glm::normalize(glm::cross(e1, e2));
	temp.d = -glm::dot(temp.N, v1);
	return temp;
}

float Plane::GetDistance(const glm::vec3& p) {
	return glm::dot(N, p) + d;
}

Plane::Where Plane::Classify(const glm::vec3& p) {
	float res = GetDistance(p);
	if (res > EPSILON)
		return FRONT;
	else if (res < EPSILON)
		return BACK;
	else
		return COPLANAR;
}
