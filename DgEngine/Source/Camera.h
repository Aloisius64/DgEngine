////////////////////////////////////////////////////////////////////////////////
// Filename: Camera.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <vector>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Component.h"
#include "Common.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;

namespace dg {
	class Configuration;
	class Frustum;
	class OculusVR;
	class SceneLoader;
	class Transform;

	class Camera : public Component {
		friend class Configuration;
		friend class Frustum;
		friend class OculusVR;

	public:
		static inline eComponentType ComponentType() {
			return eComponentType::CAMERA;
		}

	public:
		static void InitializeCamera(const string&, const float& = 0.0f, const float& = 0.0f, const float& = 1.0f, const float& = 1.0f);
		static void SetActiveCamera(Camera*);
		static Camera* ActiveCamera();
		static Camera* GetCamera(unsigned int);
		static const vector<Camera*>& GetAllCameras();

	public:
		Camera(SceneObject*,
			eProjection = eProjection::PERSPECTIVE_PROJECTION,
			float = 45.0f,
			float = 1.0f,
			float = 100.0f);
		~Camera();

		virtual void DrawUI(TwBar*);

		void ComputeMatrices();
		void UpdateViewport() const;

		void SetProjection(const eProjection&);
		const eProjection& GetProjection() const;
		void SetPosition(const vec3&);
		const vec3& GetPosition() const;
		void SetRotation(const vec3&);
		void SetRotation(const quat&);
		const vec3 GetTarget() const;
		const vec3 GetRight() const;
		const vec3 GetUp() const;
		void SetFovy(const float&);
		const float& GetFovy() const;
		void SetZNear(const float&);
		const float& GetZNear() const;
		void SetZFar(const float&);
		const float& GetZFar() const;
		void SetViewportX(const float&);
		const float& GetViewportX() const;
		void SetViewportY(const float&);
		const float& GetViewportY() const;
		void SetViewportWidth(const float&);
		const float& GetViewportWidth() const;
		void SetViewportHeight(const float&);
		const float& GetViewportHeight() const;
		const mat4& GetViewMatrix();
		const mat4& GetProjectionMatrix();
		const mat4& GetViewProjectionMatrix();
		void SetBackgroundColor(const vec4&);
		const vec4& GetBackgroundColor() const;
		void SetRenderLayer(const unsigned int&);
		const unsigned int& GetRenderLayer() const;
		
	private:
		eProjection		m_Projection;
		Transform*		m_Transform;
		float			m_Aspect;
		float			m_Fovy;
		float			m_ZNear;
		float			m_ZFar;
		float			m_ViewportX;
		float			m_ViewportY;
		float			m_ViewportWidth;
		float			m_ViewportHeight;
		mat4			m_ViewMatrix;
		mat4			m_ProjectionMatrix;
		mat4			m_ViewProjectionMatrix;
		vec4			m_BackgroundColor;
		unsigned int	m_RenderLayer;
	};
}
