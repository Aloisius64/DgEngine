////////////////////////////////////////////////////////////////////////////////
// Filename: Collider.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Collider.h"
#include "..\Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;

Collider::Collider() {
	m_ParentActor = nullptr;
	m_Material = nullptr;
	m_Center = vec_zero;
	m_Trigger = false;
	m_Shape = nullptr;
}

Collider::~Collider() {
	// Call release for this Collider in the relative subclass
	if(m_Shape) {
		m_Shape->release();
	}
	m_Material = nullptr;
}

void Collider::Initialize(const ColliderData& colliderData) {
	m_ParentActor = colliderData.parentActor;
	m_Material = colliderData.material;
	m_Center = colliderData.center;
	m_Trigger = colliderData.trigger;
}

PxActor* Collider::GetActor() {
	return m_ParentActor;
}

SceneObject* Collider::GetSceneObject() {
	return (SceneObject*) m_ParentActor->userData;
}
