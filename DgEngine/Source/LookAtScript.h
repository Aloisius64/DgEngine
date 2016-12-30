////////////////////////////////////////////////////////////////////////////////
// Filename: LookAtScript.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Script.h"
#include "Transform.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;

class LookAtScript : public Script {
public:
	LookAtScript(Transform* transform)
		: Script("LookAtScript") {
		m_TargetTransform = transform;
	}

	virtual void OnActivate() {
		m_Transform = GetComponentType(GetSceneObject(), Transform);
	}

	virtual void Update() {
		vec3 target = m_TargetTransform->GetGlobalPosition()-m_Transform->GetGlobalPosition();
		target = normalize(target);
		m_Transform->SetRotation(rotationBetweenVectors(vec_forward, target));
	}

private:
	Transform*	m_TargetTransform;
	Transform*	m_Transform;
};
