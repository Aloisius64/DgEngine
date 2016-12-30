////////////////////////////////////////////////////////////////////////////////
// Filename: BoxCollider.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "BoxCollider.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


BoxCollider::BoxCollider() : Collider() {
	m_Size = vec_one;
}

BoxCollider::~BoxCollider() {}

string BoxCollider::GetType() {
	return "BoxCollider";
}

void BoxCollider::Initialize(const ColliderData& colliderData) {
	Collider::Initialize(colliderData);
	m_Size = colliderData.sizeCollider;

	// BoxGeometry
	if(m_ParentActor->isRigidActor()) {
		PxRigidActor* rigidActor = (PxRigidStatic*) m_ParentActor;
		m_Shape = PhysX::GetPhysics()->createShape(PxBoxGeometry(m_Size[0], m_Size[1], m_Size[2]), *m_Material->GetMaterial());

		m_Shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !m_Trigger);
		m_Shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, m_Trigger);

		rigidActor->attachShape(*m_Shape);

		m_Shape->userData = this;
	}
}
