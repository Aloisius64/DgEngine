////////////////////////////////////////////////////////////////////////////////
// Filename: EnableDisableScript.h
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


class EnableDisableScript : public Script {
public:
	EnableDisableScript(Component* component, float maxDistance)
		: Script("EnableDisableScript") {
		m_Component = component;
		m_MaxDistance = maxDistance;
	}

	virtual void OnActivate() {
		m_Transform = GetComponentType(GetSceneObject(), Transform);
	}

	virtual void Update() {
		vec3 direction = m_Transform->GetPosition()-Camera::ActiveCamera()->GetPosition();
		float tetha = glm::dot(glm::normalize(direction), Camera::ActiveCamera()->GetTarget());
		if(tetha>cosf(glm::radians(160.0f))) {
			float distance = glm::length(m_Transform->GetPosition()-Camera::ActiveCamera()->GetPosition());
			m_Component->Enable(distance<m_MaxDistance);
		} else {
			m_Component->Disable();
		}
	}

private:
	float		m_MaxDistance;
	Component*	m_Component;
	Transform*	m_Transform;
};
