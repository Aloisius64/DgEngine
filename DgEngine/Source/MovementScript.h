////////////////////////////////////////////////////////////////////////////////
// Filename: Movement.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Script.h"
#include "TimeClock.h"
#include "Transform.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;

class MovementScript : public Script {
public:
	MovementScript(vec3 movement = vec_zero) 
		: Script("MovementScript") {
		m_Movement = movement;
	}

	virtual void OnActivate() {
		m_Transform = GetComponentType(GetSceneObject(), Transform);
	}

	virtual void Update() {
		static float Scale = 0.0f;
		Scale += 0.1f;
	
		m_Transform->Translate(m_Movement.x*sinf(Scale*0.2f)*0.1f,
			m_Movement.y*sinf(Scale*0.2f)*0.5f,
			m_Movement.z*sinf(Scale*0.2f)*0.5f);
	}

private:
	vec3		m_Movement;
	Transform*	m_Transform;
};
