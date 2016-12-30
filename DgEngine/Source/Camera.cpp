////////////////////////////////////////////////////////////////////////////////
// Filename: Camera.cpp
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Camera.h"
#include "Configuration.h"
#include "DataManager.h"
#include "Environment.h"
#include "Mathematics.h"
#include "SceneObject.h"
#include "Transform.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


#pragma region AntTweakBar
static void TW_CALL SetFovyCallback(const void *value, void *clientData) {
	static_cast<Camera *>(clientData)->SetFovy(*static_cast<const float *>(value));
}
static void TW_CALL GetFovyCallback(void *value, void *clientData) {
	*static_cast<float *>(value) = static_cast<Camera*>(clientData)->GetFovy();
}
static void TW_CALL SetZNearCallback(const void *value, void *clientData) {
	static_cast<Camera *>(clientData)->SetZNear(*static_cast<const float *>(value));
}
static void TW_CALL GetZNearCallback(void *value, void *clientData) {
	*static_cast<float *>(value) = static_cast<Camera*>(clientData)->GetZNear();
}
static void TW_CALL SetZFarCallback(const void *value, void *clientData) {
	static_cast<Camera *>(clientData)->SetZFar(*static_cast<const float *>(value));
}
static void TW_CALL GetZFarCallback(void *value, void *clientData) {
	*static_cast<float *>(value) = static_cast<Camera*>(clientData)->GetZFar();
}
static void TW_CALL SetViewportXCallback(const void *value, void *clientData) {
	static_cast<Camera *>(clientData)->SetViewportX(*static_cast<const float *>(value));
}
static void TW_CALL GetViewportXCallback(void *value, void *clientData) {
	*static_cast<float *>(value) = static_cast<Camera*>(clientData)->GetViewportX();
}
static void TW_CALL SetViewportYCallback(const void *value, void *clientData) {
	static_cast<Camera *>(clientData)->SetViewportY(*static_cast<const float *>(value));
}
static void TW_CALL GetViewportYCallback(void *value, void *clientData) {
	*static_cast<float *>(value) = static_cast<Camera*>(clientData)->GetViewportY();
}
static void TW_CALL SetViewportWidthCallback(const void *value, void *clientData) {
	static_cast<Camera *>(clientData)->SetViewportWidth(*static_cast<const float *>(value));
}
static void TW_CALL GetViewportWidthCallback(void *value, void *clientData) {
	*static_cast<float *>(value) = static_cast<Camera*>(clientData)->GetViewportWidth();
}
static void TW_CALL SetViewportHeightCallback(const void *value, void *clientData) {
	static_cast<Camera *>(clientData)->SetViewportHeight(*static_cast<const float *>(value));
}
static void TW_CALL GetViewportHeightCallback(void *value, void *clientData) {
	*static_cast<float *>(value) = static_cast<Camera*>(clientData)->GetViewportHeight();
}
static void TW_CALL SetBackgroundColorCallback(const void *value, void *clientData) {
	static_cast<Camera *>(clientData)->SetBackgroundColor(*static_cast<const vec4 *>(value));
}
static void TW_CALL GetBackgroundColorCallback(void *value, void *clientData) {
	*static_cast<vec4 *>(value) = static_cast<Camera*>(clientData)->GetBackgroundColor();
}
static void TW_CALL SetRenderLayerrCallback(const void *value, void *clientData) {
	static_cast<Camera *>(clientData)->SetRenderLayer(*static_cast<const unsigned int *>(value));
}
static void TW_CALL GetRenderLayerCallback(void *value, void *clientData) {
	*static_cast<unsigned int *>(value) = static_cast<Camera*>(clientData)->GetRenderLayer();
}
static void TW_CALL SetProjectionCallback(const void *value, void *clientData) {
	static_cast<Camera *>(clientData)->SetProjection(*static_cast<const eProjection *>(value));
}
static void TW_CALL GetProjectionCallback(void *value, void *clientData) {
	*static_cast<eProjection *>(value) = static_cast<Camera*>(clientData)->GetProjection();
}
#pragma endregion

void Camera::InitializeCamera(const string& name, const float& viewportX, const float& viewportY, const float& viewportWidth, const float& viewportHeight) {
	SceneObject* sceneObject = new SceneObject(name, "MainCamera");
	Transform* transform = new Transform(sceneObject, vec_up*20.0f + vec_forward*50.0f, vec_right*20.0f + vec_up*180.0f);
	Camera* camera = new Camera(sceneObject);
	camera->SetViewportX(viewportX);
	camera->SetViewportY(viewportY);
	camera->SetViewportWidth(viewportWidth);
	camera->SetViewportHeight(viewportHeight);
	camera->SetZFar(1000.0f);

	SceneObject::Root()->AddChild(sceneObject);
}

void Camera::SetActiveCamera(Camera* camera) {
	DataManager::Instance()->m_MainCamera = camera;
}

Camera* Camera::ActiveCamera() {
	return DataManager::Instance()->m_MainCamera;
}

Camera* Camera::GetCamera(unsigned int camera) {
	if (camera >= 0 && camera < DataManager::Instance()->m_Cameras.size())
		return DataManager::Instance()->m_Cameras[camera];
	return NULL;
}

const vector<Camera*>& Camera::GetAllCameras() {
	return DataManager::Instance()->m_Cameras;
}

Camera::Camera(SceneObject* sceneObject,
	eProjection projection,
	float fovy,
	float zNear,
	float zFar)
	: Component(eComponentType::CAMERA, sceneObject),
	m_Projection(projection),
	m_Fovy(fovy),
	m_ZNear(zNear),
	m_ZFar(zFar),
	m_BackgroundColor(color_black) {

	if (!DataManager::Instance()->m_MainCamera) {
		DataManager::Instance()->m_MainCamera = this;
	}
	DataManager::Instance()->m_Cameras.push_back(this);

	// Set a render layer.
	SetRenderLayer(DataManager::Instance()->m_Cameras.size() - 1);

	m_Transform = GetComponentType(sceneObject, Transform);

	m_ViewportX = 0.0f;
	m_ViewportY = 0.0f;
	m_ViewportWidth = 1.0f;
	m_ViewportHeight = 1.0f;
	m_Aspect = (m_ViewportWidth * Configuration::Instance()->GetScreenWidth()) / (m_ViewportHeight * Configuration::Instance()->GetScreenHeigth());
	m_ViewMatrix = mat4(1.0f);
	m_ProjectionMatrix = mat4(1.0f);
}

Camera::~Camera() {
	// Remove camera references in DataManager.
	if (DataManager::Instance()->m_MainCamera == this) {
		DataManager::Instance()->m_MainCamera = nullptr;
	}

	for (vector<Camera*>::iterator it = DataManager::Instance()->m_Cameras.begin();
		it != DataManager::Instance()->m_Cameras.end(); it++) {
		if ((*it) == this) {
			DataManager::Instance()->m_Cameras.erase(it);
			break;
		}
	}
}

void Camera::DrawUI(TwBar* bar) {
	char parameter[128];
	memset(parameter, 0, sizeof(parameter));

	// Camera component values.
	TwAddButton(bar, "Camera", NULL, NULL, "");
	TwAddVarCB(bar, "Camera Enabled", TW_TYPE_BOOL8, EnabledCallback, IsEnabledCallback, this, "");
	TwAddVarCB(bar, "CameraProjection", TW_TYPE_CAMERA_PROJECTION, SetProjectionCallback, GetProjectionCallback, this, " label='Projection' ");
	TwAddVarCB(bar, "Fovy", TW_TYPE_FLOAT, SetFovyCallback, GetFovyCallback, this, "");
	SNPRINTF(parameter, sizeof(parameter), "%s/Fovy min=%d max=%d ", GetSceneObject()->Id().c_str(), 0, 90);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/Fovy step=%d ", GetSceneObject()->Id().c_str(), 1);
	TwDefine(parameter);
	TwAddVarCB(bar, "NearPlane", TW_TYPE_FLOAT, SetZNearCallback, GetZNearCallback, this, "");
	SNPRINTF(parameter, sizeof(parameter), "%s/NearPlane min=%g ", GetSceneObject()->Id().c_str(), 0.1f);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/NearPlane step=%g ", GetSceneObject()->Id().c_str(), 0.1f);
	TwDefine(parameter);
	TwAddVarCB(bar, "FarPlane", TW_TYPE_FLOAT, SetZFarCallback, GetZFarCallback, this, "");
	SNPRINTF(parameter, sizeof(parameter), "%s/FarPlane max=%g ", GetSceneObject()->Id().c_str(), 1000.0f);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/FarPlane step=%g ", GetSceneObject()->Id().c_str(), 0.1f);
	TwDefine(parameter);
	TwAddButton(bar, "Viewport", NULL, NULL, "");
	TwAddVarCB(bar, "X", TW_TYPE_FLOAT, SetViewportXCallback, GetViewportXCallback, this, "");
	SNPRINTF(parameter, sizeof(parameter), "%s/X min=%g max=%g ", GetSceneObject()->Id().c_str(), 0.0f, 1.0f);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/X step=%g ", GetSceneObject()->Id().c_str(), 0.1f);
	TwDefine(parameter);
	TwAddVarCB(bar, "Y", TW_TYPE_FLOAT, SetViewportYCallback, GetViewportYCallback, this, "");
	SNPRINTF(parameter, sizeof(parameter), "%s/Y min=%g max=%g ", GetSceneObject()->Id().c_str(), 0.0f, 1.0f);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/Y step=%g ", GetSceneObject()->Id().c_str(), 0.1f);
	TwDefine(parameter);
	TwAddVarCB(bar, "Width", TW_TYPE_FLOAT, SetViewportWidthCallback, GetViewportWidthCallback, this, "");
	SNPRINTF(parameter, sizeof(parameter), "%s/Width min=%g max=%g ", GetSceneObject()->Id().c_str(), 0.0f, 1.0f);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/Width step=%g ", GetSceneObject()->Id().c_str(), 0.1f);
	TwDefine(parameter);
	TwAddVarCB(bar, "Height", TW_TYPE_FLOAT, SetViewportHeightCallback, GetViewportHeightCallback, this, "");
	SNPRINTF(parameter, sizeof(parameter), "%s/Height min=%g max=%g ", GetSceneObject()->Id().c_str(), 0.0f, 1.0f);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/Height step=%g ", GetSceneObject()->Id().c_str(), 0.1f);
	TwDefine(parameter);
	TwAddVarCB(bar, "Backgroud color", TW_TYPE_COLOR4F, SetBackgroundColorCallback, GetBackgroundColorCallback, this, "");
	TwAddVarCB(bar, "CameraRenderLayer", TW_TYPE_UINT32, SetRenderLayerrCallback, GetRenderLayerCallback, this, " label='RenderLayer' ");
	SNPRINTF(parameter, sizeof(parameter), "%s/CameraRenderLayer min=%d ", GetSceneObject()->Id().c_str(), 0);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/CameraRenderLayer step=%d ", GetSceneObject()->Id().c_str(), 1);
	TwDefine(parameter);
	TwAddSeparator(bar, "", NULL);
	TwAddSeparator(bar, "", NULL);
}

void Camera::ComputeMatrices() {
	if (!Environment::Instance()->IsOVRActive()) {
		// View matrix
		m_ViewMatrix = glm::lookAt(GetPosition(), GetPosition() + GetTarget(), GetUp());

		// Projection matrix
		if (m_Projection == eProjection::ORTHOGRAPHIC_PROJECTION) {
			m_ProjectionMatrix = glm::ortho(
				-50.0f, 50.0f,
				-50.0f, 50.0f,
				m_ZNear, m_ZFar);
		}
		else {
			m_Aspect = (m_ViewportWidth * Configuration::Instance()->GetScreenWidth()) / (m_ViewportHeight * Configuration::Instance()->GetScreenHeigth());
			m_ProjectionMatrix = glm::perspective(glm::radians(m_Fovy), m_Aspect, m_ZNear, m_ZFar);
		}
	}

	// View-Projection matrix
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void Camera::UpdateViewport() const {
	glViewport((GLint)(m_ViewportX * Configuration::Instance()->GetScreenWidth()),
		(GLint)(m_ViewportY * Configuration::Instance()->GetScreenWidth()),
		(GLsizei)(m_ViewportWidth * Configuration::Instance()->GetScreenWidth()),
		(GLsizei)(m_ViewportHeight * Configuration::Instance()->GetScreenHeigth()));
}

void Camera::SetProjection(const eProjection& value) {
	m_Projection = value;
}

const eProjection& Camera::GetProjection() const {
	return m_Projection;
}

void Camera::SetPosition(const vec3& value) {
	m_Transform->SetPosition(value);
}

const vec3& Camera::GetPosition() const {
	return m_Transform->GetPosition();
}

void Camera::SetRotation(const vec3& value) {
	m_Transform->SetRotation(value);
}

void Camera::SetRotation(const quat& value) {
	m_Transform->SetRotation(value);
}

const vec3 Camera::GetTarget() const {
	//return dg::DirectionFromRotation(-vec_forward, m_Transform->GetRotation());
	return dg::DirectionFromRotation(vec_forward, m_Transform->GetRotation());
}

const vec3 Camera::GetRight() const {
	return glm::normalize(glm::cross(vec_up, GetTarget()));
}

const vec3 Camera::GetUp() const {
	return glm::cross(GetTarget(), GetRight());
	//return dg::UpDirectionFromRotation(m_Transform->GetRotation());
	//return vec_up;
}

void Camera::SetFovy(const float& value) {
	m_Fovy = value;
}

const float& Camera::GetFovy() const {
	return m_Fovy;
}

void Camera::SetZNear(const float& value) {
	m_ZNear = value;
}

const float& Camera::GetZNear() const {
	return m_ZNear;
}

void Camera::SetZFar(const float& value) {
	m_ZFar = value;
}

const float& Camera::GetZFar() const {
	return m_ZFar;
}

void Camera::SetViewportX(const float& value) {
	m_ViewportX = value;
}

const float& Camera::GetViewportX() const {
	return m_ViewportX;
}

void Camera::SetViewportY(const float& value) {
	m_ViewportY = value;
}

const float& Camera::GetViewportY() const {
	return m_ViewportY;
}

void Camera::SetViewportWidth(const float& value) {
	m_ViewportWidth = value;
}

const float& Camera::GetViewportWidth() const {
	return m_ViewportWidth;
}

void Camera::SetViewportHeight(const float& value) {
	m_ViewportHeight = value;
}

const float& Camera::GetViewportHeight() const {
	return m_ViewportHeight;
}

const mat4& Camera::GetViewMatrix() {
	return m_ViewMatrix;
}

const mat4& Camera::GetProjectionMatrix() {
	return m_ProjectionMatrix;
}

const mat4& Camera::GetViewProjectionMatrix() {
	return m_ViewProjectionMatrix;
}

void Camera::SetBackgroundColor(const vec4& value) {
	m_BackgroundColor = value;
}

const vec4& Camera::GetBackgroundColor() const {
	return m_BackgroundColor;
}

void Camera::SetRenderLayer(const unsigned int& value) {
	m_RenderLayer = value;
}

const unsigned int& Camera::GetRenderLayer() const {
	return m_RenderLayer;
}
