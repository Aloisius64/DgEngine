////////////////////////////////////////////////////////////////////////////////
// Filename: TrackingCameraScript.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Script.h"
#include "Camera.h"
#include "Transform.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


class TrackingCameraScript : public Script {
public:
	TrackingCameraScript(SceneObject* target, float distanceToTarget = 20.0f)
		: Script("TrackingCameraScript") {
		m_Target = target;
		m_Camera = Camera::ActiveCamera();
		m_DistanceToTarget = distanceToTarget;
	}

	virtual void OnActivate() {
		m_TargetTransform = GetComponentType(m_Target, Transform);
	}

	virtual void Update() {
		vec3 newPosition = m_TargetTransform->GetPosition();
		newPosition.z -= m_DistanceToTarget;
		newPosition.y += 5.0f;
		m_Camera->SetPosition(newPosition);
	}

private:
	SceneObject*	m_Target;
	Transform*		m_TargetTransform;
	Camera*			m_Camera;
	float			m_DistanceToTarget;
};
