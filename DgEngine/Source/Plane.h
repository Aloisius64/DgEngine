////////////////////////////////////////////////////////////////////////////////
// Filename: Plane.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <algorithm>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Mathematics.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class Frustum;

	class Plane {
		friend class Frustum;

		enum Where { COPLANAR, FRONT, BACK };

	public:
		Plane(void);
		Plane(const glm::vec3& N, const glm::vec3& p);
		~Plane(void);

		static Plane FromPoints(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
		Where Classify(const glm::vec3& p);
		float GetDistance(const glm::vec3& p);
		
	private:
		glm::vec3	N;
		float		d;
	};
}
