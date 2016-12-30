////////////////////////////////////////////////////////////////////////////////
// Filename: PlaneCollider.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "PlaneCollider.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


PlaneCollider::PlaneCollider() : Collider() {
	m_Direction = eGeometryDirection::Y_AXIS;
}

PlaneCollider::~PlaneCollider() {

}

string PlaneCollider::GetType() {
	return "PlaneCollider";
}

void PlaneCollider::Initialize(const ColliderData& colliderData) {
	Collider::Initialize(colliderData);
	m_Direction = colliderData.direction;

	// PlaneGeometry
	if(m_ParentActor->isRigidActor()) {
		PxRigidActor* rigidActor = (PxRigidStatic*) m_ParentActor;
		m_Shape = PhysX::GetPhysics()->createShape(PxPlaneGeometry(), *m_Material->GetMaterial());
		PxTransform relativePose;

		switch(m_Direction) {
		case eGeometryDirection::X_AXIS:
			relativePose = PxTransform(PxQuat(PxHalfPi, PxVec3(0, 1, 0)));
			m_Shape->setLocalPose(relativePose);
			break;
		case eGeometryDirection::Z_AXIS:
			relativePose = PxTransform(PxQuat(PxHalfPi, PxVec3(1, 0, 0)));
			m_Shape->setLocalPose(relativePose);
			break;
		case eGeometryDirection::Y_AXIS:
			relativePose = PxTransform(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
			m_Shape->setLocalPose(relativePose);
			break;
		default:
			break;
		}

		m_Shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !m_Trigger);
		m_Shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, m_Trigger);

		rigidActor->attachShape(*m_Shape);

		m_Shape->userData = this;
	}
}

