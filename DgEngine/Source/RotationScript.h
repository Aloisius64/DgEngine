////////////////////////////////////////////////////////////////////////////////
// Filename: RotationScript.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Script.h"
#include "System.h"
#include "TimeClock.h"
#include "Transform.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;

class RotationScript : public Script {
public:
	RotationScript(vec3 rotation = vec_zero) 
		: Script("RotationScript") {
		m_Rotation = rotation;
		m_Transform = nullptr;
	}
	
	virtual void OnActivate() {
		m_Transform = GetComponentType(GetSceneObject(), Transform);
	}

	virtual void Update() {
		m_Transform->Rotate(m_Rotation*TimeClock::Instance()->GetDeltaTime());
	}

private:
	vec3		m_Rotation;
	Transform*	m_Transform;
};
