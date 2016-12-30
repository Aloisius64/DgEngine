////////////////////////////////////////////////////////////////////////////////
// Filename: Transform.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <list>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Component.h"
#include "Mathematics.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {

	class Transform : public Component {

	public:
		static inline eComponentType ComponentType() {
			return eComponentType::TRANSFORM;
		}

	public:
		Transform(SceneObject*, vec3 = vec_zero, vec3 = vec_zero, vec3 = vec_one);

		virtual ~Transform();

		virtual void Update();
		virtual void DrawUI(TwBar*);

		const mat4& GetWorldMatrix();
		const mat4& GetViewMatrix();
		const mat4& GetProjectionMatrix();
		const mat4& GetWorldViewProjectionMatrix();
		const mat4& GetWorldProjectionMatrix();
		const mat4& GetWorldViewMatrix();
		const mat4& GetViewProjectionMatrix();

		void SetPosition(float x, float y, float z);
		void SetPosition(vec3 position);
		void Translate(float x, float y, float z);
		void Translate(vec3 position);
		const vec3& GetPosition() const;
		vec3 GetGlobalPosition();
		void SetRotation(float x, float y, float z);
		void SetRotation(vec3 rotation);
		void SetRotation(quat);
		void Rotate(float x, float y, float z);
		void Rotate(vec3 rotation);
		void Rotate(quat);
		const quat& GetRotation() const;
		void SetScaling(float x, float y, float z);
		void SetScaling(vec3 scale);
		void Scale(float s);
		void Scale(float x, float y, float z);
		void Scale(vec3 scale);
		const vec3& GetScaling() const;

		Transform operator+(const Transform& transform);
		Transform operator+=(const Transform& transform);
		Transform operator=(const Transform& transform);
		bool operator==(const Transform& transform);
		bool operator!=(const Transform& transform);

	private:
		void ComputeTranslationMatrix();
		void ComputeRotationMatrix();
		void ComputeScalingMatrix();
		void ComputeWorldMatrix();

	private:
		vec3		m_Position;
		quat		m_Rotation;
		vec3		m_Scaling;
		mat4		m_TranslationMatrix;
		mat4		m_RotationMatrix;
		mat4		m_ScalingMatrix;
		mat4		m_WMatrix;
		mat4		m_WPMatrix;
		mat4		m_WVMatrix;
		mat4		m_VPMatrix;
		mat4		m_WVPMatrix;
		vec3		m_ParentPosition;
		quat		m_ParentRotation;
		vec3		m_ParentScaling;
	};
}
