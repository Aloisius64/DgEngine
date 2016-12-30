////////////////////////////////////////////////////////////////////////////////
// Filename: SphereCollider.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "SphereCollider.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


SphereCollider::SphereCollider() : Collider() {
	m_Radius = 0.5f;
}

SphereCollider::~SphereCollider() {

}

string SphereCollider::GetType() {
	return "SphereCollider";
}

void SphereCollider::Initialize(const ColliderData& colliderData) {
	Collider::Initialize(colliderData);
	m_Radius = colliderData.radius;

	// SphereGeometry
	if(m_ParentActor->isRigidActor()) {
		PxRigidActor* rigidActor = (PxRigidStatic*) m_ParentActor;
		m_Shape = PhysX::GetPhysics()->createShape(PxSphereGeometry(m_Radius), *m_Material->GetMaterial());

		m_Shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !m_Trigger);
		m_Shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, m_Trigger);

		rigidActor->attachShape(*m_Shape);

		m_Shape->userData = this;
	}
}
