////////////////////////////////////////////////////////////////////////////////
// Filename: RotationAboutTargetScript.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Script.h"
#include "Mathematics.h"
#include "Transform.h"
#include "System.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


class RotationAboutTargetScript : public Script {
public:
	RotationAboutTargetScript(Transform* target, float increment)
		: Script("RotationAboutTargetScript") {
		m_ThisTransform = nullptr;
		m_Target = target;
		m_Rotation = 0.1f;
		m_Increment = increment;
	}

	virtual void OnActivate() {
		m_ThisTransform = GetComponentType(GetSceneObject(), Transform);
		m_Diff = m_Target->GetPosition()-m_ThisTransform->GetPosition();
	}

	virtual void Update() {
		vec3 e_rotation(0.0f, m_Rotation, 0.0f);
		quat q_rotation = EulerAngleToQuaternion(e_rotation);

		vec3 offsetPosition = glm::rotate(q_rotation, m_Diff);
		m_ThisTransform->SetPosition(m_Target->GetPosition()+offsetPosition);
		m_Rotation += m_Increment*TimeClock::Instance()->GetDeltaTime();
	}

private:
	Transform*	m_ThisTransform;
	Transform*	m_Target;
	vec3		m_Diff;
	float		m_Rotation;
	float		m_Increment;
};
