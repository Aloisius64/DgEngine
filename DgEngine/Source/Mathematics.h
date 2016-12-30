////////////////////////////////////////////////////////////////////////////////
// Filename: Mathematics.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
//#include <vmath.h>
#include <glm/glm.hpp>

#pragma region GTC
#include <glm/gtc/bitfield.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/integer.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_integer.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/packing.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/reciprocal.hpp>
#include <glm/gtc/round.hpp>
#include <glm/gtc/type_precision.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/ulp.hpp>
#include <glm/gtc/vec1.hpp>
#pragma endregion

#pragma region GTX
//#include <glm/gtx/associated_min_max.hpp>
//#include <glm/gtx/bit.hpp>
//#include <glm/gtx/closest_point.hpp>
#include <glm/gtx/color_space.hpp>
//#include <glm/gtx/color_space_YCoCg.hpp>
//#include <glm/gtx/common.hpp>
//#include <glm/gtx/compatibility.hpp>
//#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
//#include <glm/gtx/extend.hpp>
//#include <glm/gtx/extented_min_max.hpp>
//#include <glm/gtx/fast_exponential.hpp>
//#include <glm/gtx/fast_square_root.hpp>
//#include <glm/gtx/fast_trigonometry.hpp>
//#include <glm/gtx/gradient_paint.hpp>
//#include <glm/gtx/handed_coordinate_space.hpp>
//#include <glm/gtx/integer.hpp>
//#include <glm/gtx/intersect.hpp>
////#include <glm/gtx/io.hpp>
//#include <glm/gtx/log_base.hpp>
#include <glm/gtx/matrix_cross_product.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtx/matrix_major_storage.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/matrix_query.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/mixed_product.hpp>
//#include <glm/gtx/multiple.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtx/normalize_dot.hpp>
//#include <glm/gtx/number_precision.hpp>
//#include <glm/gtx/optimum_pow.hpp>
#include <glm/gtx/orthonormalize.hpp>
#include <glm/gtx/perpendicular.hpp>
#include <glm/gtx/polar_coordinates.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/quaternion.hpp>
//#include <glm/gtx/range.hpp>
//#include <glm/gtx/raw_data.hpp>
#include <glm/gtx/rotate_normalized_axis.hpp>
#include <glm/gtx/rotate_vector.hpp>
//#include <glm/gtx/scalar_multiplication.hpp>
//#include <glm/gtx/scalar_relational.hpp>
#include <glm/gtx/simd_mat4.hpp>
#include <glm/gtx/simd_quat.hpp>
#include <glm/gtx/simd_vec4.hpp>
//#include <glm/gtx/spline.hpp>
//#include <glm/gtx/std_based_type.hpp>
//#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/type_aligned.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/vector_query.hpp>
//#include <glm/gtx/wrap.hpp>
#pragma endregion

////////////////
// NAMESPACES //
////////////////
using namespace glm;
//using namespace vmath;


#define COMPONENT_MULTIPLY(result, a, b)\
	result.x = a.x * b.x;\
	result.y = a.y * b.y;\
	result.z = a.z * b.z;

static inline quat EulerAngleToQuaternion(vec3 eulerAngle) {
	return quat(radians(eulerAngle));
}

static inline vec3 QuaternionToEulerAngle(quat q) {

	//float pitch = atan2f(2.0f*(q.w*q.x+q.y*q.z), 1.0f-2.0f*(q.x*q.x+q.y*q.y));
	//float yaw = asinf(2.0f*q.w*q.y-q.z*q.x);
	//float roll = atan2f(2.0f*(q.w*q.z+q.x*q.y), 1.0f-2.0f*(q.y*q.y+q.z*q.z));

	float pitch = glm::pitch(q);
	float yaw = glm::yaw(q);
	float roll = glm::roll(q);

	return degrees(vec3(pitch, yaw, roll));
}

static inline quat rotationBetweenVectors(vec3 start, vec3 dest) {
	start = normalize(start);
	dest = normalize(dest);

	float cosTheta = dot(start, dest);
	vec3 rotationAxis;

	if(cosTheta<-1+0.001f) {
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotationAxis = cross(vec3(0.0f, 0.0f, 1.0f), start);
		if(dot(rotationAxis, rotationAxis)<0.01) // bad luck, they were parallel, try again!
			rotationAxis = cross(vec3(1.0f, 0.0f, 0.0f), start);

		rotationAxis = normalize(rotationAxis);
		return angleAxis(180.0f, rotationAxis);
	}

	rotationAxis = cross(start, dest);

	float s = sqrt((1+cosTheta)*2);
	float invs = 1/s;

	return quat(
		s * 0.5f,
		rotationAxis[0]*invs,
		rotationAxis[1]*invs,
		rotationAxis[2]*invs);
}

static inline quat lookAt(vec3 direction, vec3 desiredUp) {
	if(dot(direction, direction)<0.0001f)
		return quat();

	// Recompute desiredUp so that it's perpendicular to the direction
	// You can skip that part if you really want to force desiredUp
	vec3 right = cross(direction, desiredUp);
	desiredUp = cross(right, direction);

	// Find the rotation between the front of the object (that we assume towards +Z,
	// but this depends on your model) and the desired direction
	quat rot1 = rotationBetweenVectors(vec3(0.0f, 0.0f, 1.0f), direction);
	// Because of the 1rst rotation, the up is probably completely screwed up. 
	// Find the rotation between the "up" of the rotated object, and the desired up
	vec3 newUp = rot1 * vec3(0.0f, 0.0f, 1.0f) * conjugate(rot1);
	quat rot2 = rotationBetweenVectors(newUp, desiredUp);

	// Apply them
	return rot2 * rot1; // remember, in reverse order.
}
