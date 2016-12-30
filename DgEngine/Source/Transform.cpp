////////////////////////////////////////////////////////////////////////////////
// Filename: Transform.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Transform.h"
#include "Camera.h"
#include "DataManager.h"
#include "Gizmo.h"
#include "Mathematics.h"
#include "SceneObject.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


#pragma region AntTweakBar
static void TW_CALL SetPositionCallback(const void *value, void *clientData) {
	static_cast<Transform *>(clientData)->SetPosition(*static_cast<const vec3 *>(value));
}
static void TW_CALL GetPositionCallback(void *value, void *clientData) {
	*static_cast<vec3 *>(value) = static_cast<Transform*>(clientData)->GetPosition();
}
static void TW_CALL SetEulerRotationCallback(const void *value, void *clientData) {
	static_cast<Transform *>(clientData)->SetRotation(*static_cast<const vec3 *>(value));
}
static void TW_CALL GetEulerRotationCallback(void *value, void *clientData) {
	quat q_rotation = static_cast<Transform*>(clientData)->GetRotation();
	*static_cast<vec3 *>(value) = QuaternionToEulerAngle(q_rotation);
}
static void TW_CALL SetRotationCallback(const void *value, void *clientData) {
	static_cast<Transform *>(clientData)->SetRotation(*static_cast<const quat *>(value));
}
static void TW_CALL GetRotationCallback(void *value, void *clientData) {
	*static_cast<quat *>(value) = static_cast<Transform*>(clientData)->GetRotation();
}
static void TW_CALL SetScalingCallback(const void *value, void *clientData) {
	static_cast<Transform *>(clientData)->SetScaling(*static_cast<const vec3 *>(value));
}
static void TW_CALL GetScalingCallback(void *value, void *clientData) {
	*static_cast<vec3 *>(value) = static_cast<Transform*>(clientData)->GetScaling();
}
#pragma endregion

Transform::Transform(SceneObject* sceneObject, vec3 position, vec3 rotation, vec3 scaling)
	: Component(eComponentType::TRANSFORM, sceneObject),
	m_Position(position),
	m_Rotation(EulerAngleToQuaternion(rotation)),
	m_Scaling(scaling) {
		
	ComputeTranslationMatrix();
	ComputeRotationMatrix();
	ComputeScalingMatrix();
}

Transform::~Transform() {

}

void Transform::Update() {
	ComputeWorldMatrix();	// Update World matrix.
}

void Transform::DrawUI(TwBar* bar) {
	char parameter[128];
	memset(parameter, 0, sizeof(parameter));

	// Transform component values.
	TwAddButton(bar, "Transform", NULL, NULL, "");
	TwAddVarCB(bar, "Transform Enabled", TW_TYPE_BOOL8, EnabledCallback, IsEnabledCallback, this, "");
	TwAddVarCB(bar, "Position", TW_TYPE_VEC3, SetPositionCallback, GetPositionCallback, this, "");
	TwAddVarCB(bar, "Euler Rotation", TW_TYPE_VEC3, SetEulerRotationCallback, GetEulerRotationCallback, this, "");
	TwAddVarCB(bar, "Rotation", TW_TYPE_QUAT4F, SetRotationCallback, GetRotationCallback, this, "");
	SNPRINTF(parameter, sizeof(parameter), "%s/Rotation showval=true ", GetSceneObject()->Id().c_str());
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/Rotation axisx=x axisy=y axisz=z ", GetSceneObject()->Id().c_str());
	TwDefine(parameter);
	TwAddVarCB(bar, "Scaling", TW_TYPE_VEC3, SetScalingCallback, GetScalingCallback, this, "");
	TwAddSeparator(bar, "", NULL);
	TwAddSeparator(bar, "", NULL);
}

void Transform::ComputeTranslationMatrix() {
	m_TranslationMatrix = glm::translate(mat4(1.0f), m_Position);
}

void Transform::ComputeRotationMatrix() {
	m_RotationMatrix = glm::mat4_cast(m_Rotation);
}

void Transform::ComputeScalingMatrix() {
	m_ScalingMatrix = glm::scale(mat4(1.0f), m_Scaling);
}

void Transform::ComputeWorldMatrix() {
	//Transform* parentTransform = nullptr;
	//if(GetSceneObject()&&GetSceneObject()->Parent()) {
	//	parentTransform = GetComponentType(GetSceneObject()->Parent(), Transform);
	//}
	//if(parentTransform) {
	//	for(size_t i = 0; i<3; i++) {
	//		//m_ParentScaling[i] = parentTransform->GetGlobalScaling()[i]*m_Scaling[i];
	//		m_ParentScaling[i] = parentTransform->GetScaling()[i]*m_Scaling[i];
	//	}
	//	// Computing my parent transformation matrix, usefull for child object
	//	m_ParentTransformMatrix =
	//		parentTransform->m_ParentTransformMatrix *
	//		m_TranslationMatrix *
	//		m_RotationMatrix;
	//	// Computing world matrix
	//	m_WMatrix =
	//		m_ParentTransformMatrix *
	//		glm::scale(mat4(1.0f), m_ParentScaling);
	//} else {
	//	m_ParentScaling = m_Scaling;
	//	m_WMatrix = m_TranslationMatrix * m_RotationMatrix * m_ScalingMatrix;
	//	m_ParentTransformMatrix =
	//		m_TranslationMatrix *
	//		m_RotationMatrix;
	//}

	if (GetSceneObject() && GetSceneObject()->Parent()) {
		Transform* parentTransform = GetComponentType(GetSceneObject()->Parent(), Transform);

		// Computing new position, rotation and scaling to implement parenting.
		m_ParentPosition = glm::rotate(parentTransform->m_Rotation, m_Position);
		m_ParentRotation = parentTransform->m_Rotation*m_Rotation;
		COMPONENT_MULTIPLY(m_ParentScaling, parentTransform->GetScaling(), m_Scaling);

		m_TranslationMatrix = glm::translate(mat4(1.0f), parentTransform->m_ParentPosition + m_ParentPosition);
		m_RotationMatrix = glm::mat4_cast(m_ParentRotation);
		m_ScalingMatrix = glm::scale(mat4(1.0f), m_ParentScaling);
	}
	else {
		m_ParentPosition = m_Position;
		m_ParentRotation = m_Rotation;
		m_ParentScaling = m_Scaling;
	}
	m_WMatrix = m_TranslationMatrix * m_RotationMatrix * m_ScalingMatrix;
}

const mat4& Transform::GetWorldMatrix() {
	return m_WMatrix;
}

const mat4& Transform::GetViewMatrix() {
	return Camera::ActiveCamera()->GetViewMatrix();
}

const mat4& Transform::GetProjectionMatrix() {
	return Camera::ActiveCamera()->GetProjectionMatrix();
}

const mat4& Transform::GetWorldViewProjectionMatrix() {
	m_WVPMatrix = Camera::ActiveCamera()->GetViewProjectionMatrix() * GetWorldMatrix();
	return m_WVPMatrix;
}

const mat4& Transform::GetWorldProjectionMatrix() {
	m_WPMatrix = GetProjectionMatrix() * m_WMatrix;
	return m_WPMatrix;
}

const mat4& Transform::GetWorldViewMatrix() {
	m_WVMatrix = GetViewMatrix() * m_WMatrix;
	return m_WVMatrix;
}

const mat4& Transform::GetViewProjectionMatrix() {
	m_VPMatrix = Camera::ActiveCamera()->GetViewProjectionMatrix();
	return m_VPMatrix;
}

void Transform::SetPosition(float x, float y, float z) {
	m_Position = vec3(x, y, z);
	ComputeTranslationMatrix();
}

void Transform::SetPosition(vec3 position) {
	m_Position = position;
	ComputeTranslationMatrix();
}

void Transform::Translate(float x, float y, float z) {
	m_Position += vec3(x, y, z);
	ComputeTranslationMatrix();
}

void Transform::Translate(vec3 position) {
	m_Position += position;
	ComputeTranslationMatrix();
}

const vec3& Transform::GetPosition() const {
	return m_Position;
}

vec3 Transform::GetGlobalPosition() {
	return vec3(m_WMatrix[3][0], m_WMatrix[3][1], m_WMatrix[3][2]);
}

void Transform::SetRotation(float x, float y, float z) {
	SetRotation(EulerAngleToQuaternion(vec3(x, y, z)));
}

void Transform::SetRotation(vec3 rotation) {
	SetRotation(EulerAngleToQuaternion(rotation));
}

void Transform::SetRotation(quat rotation) {
	m_Rotation = rotation;
	ComputeRotationMatrix();
}

void Transform::Rotate(float x, float y, float z) {
	Rotate(EulerAngleToQuaternion(vec3(x, y, z)));
}

void Transform::Rotate(vec3 rotation) {
	Rotate(EulerAngleToQuaternion(rotation));
}

void Transform::Rotate(quat rotation) {
	m_Rotation = m_Rotation * rotation;
	ComputeRotationMatrix();
}

const quat& Transform::GetRotation() const {
	return m_Rotation;
}

void Transform::SetScaling(float x, float y, float z) {
	m_Scaling = vec3(x, y, z);
	ComputeScalingMatrix();
}

void Transform::SetScaling(vec3 scale) {
	m_Scaling = scale;
	ComputeScalingMatrix();
}

void Transform::Scale(float s) {
	Scale(s, s, s);
	ComputeScalingMatrix();
}

void Transform::Scale(float x, float y, float z) {
	vec3 scale(x, y, z);
	for (size_t i = 0; i < 3; i++) {
		m_Scaling[i] *= scale[i];
	}
	ComputeScalingMatrix();
}

void Transform::Scale(vec3 scale) {
	for (size_t i = 0; i < 3; i++) {
		m_Scaling[i] *= scale[i];
	}
	ComputeScalingMatrix();
}

const vec3& Transform::GetScaling() const {
	return m_Scaling;
}

Transform Transform::operator+(const Transform& transform) {
	m_Position = m_Position + transform.m_Position;
	m_Rotation = m_Rotation + transform.m_Rotation;
	m_Scaling = m_Scaling + transform.m_Scaling;
	return *this;
}

Transform Transform::operator+=(const Transform& transform) {
	m_Position += transform.m_Position;
	m_Rotation += transform.m_Rotation;
	m_Scaling += transform.m_Scaling;
	return *this;
}

Transform Transform::operator=(const Transform& transform) {
	m_Position = transform.m_Position;
	m_Rotation = transform.m_Rotation;
	m_Scaling = transform.m_Scaling;
	return *this;
}

bool Transform::operator==(const Transform& transform) {
	return (m_Position == transform.m_Position)
		&& (m_Rotation == transform.m_Rotation)
		&& (m_Scaling == transform.m_Scaling);
}

bool Transform::operator!=(const Transform& transform) {
	return !((*this) == transform);
}
