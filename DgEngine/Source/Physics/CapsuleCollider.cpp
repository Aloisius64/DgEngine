////////////////////////////////////////////////////////////////////////////////
// Filename: CapsuleCollider.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "CapsuleCollider.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


CapsuleCollider::CapsuleCollider() : Collider() {
	m_Radius = 0.5f;
	m_Height = 1.0f;
	m_Direction = eGeometryDirection::Y_AXIS;
}

CapsuleCollider::~CapsuleCollider() {

}

string CapsuleCollider::GetType() {
	return "CapsuleCollider";
}

void CapsuleCollider::Initialize(const ColliderData& colliderData) {
	Collider::Initialize(colliderData);
	m_Radius = colliderData.radius;
	m_Height = colliderData.height;
	m_Direction = colliderData.direction;

	// CapsuleGeometry
	if(m_ParentActor->isRigidActor()) {
		PxRigidActor* rigidActor = (PxRigidStatic*) m_ParentActor;
		m_Shape = PhysX::GetPhysics()->createShape(PxCapsuleGeometry(m_Radius, m_Height), *m_Material->GetMaterial());

		switch(m_Direction) {
		case eGeometryDirection::X_AXIS:{
			PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 1, 0)));
			m_Shape->setLocalPose(relativePose);
		} break;
			//case DgGeometryDirection::Z_AXIS:
			//	//PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 0)));
			//	//shape->setLocalPose(relativePose); 
			//	break;
		case eGeometryDirection::Y_AXIS: {
			PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
			m_Shape->setLocalPose(relativePose);
		} break;
		default:
			break;
		}

		m_Shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !m_Trigger);
		m_Shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, m_Trigger);

		rigidActor->attachShape(*m_Shape);

		m_Shape->userData = this;
	}
}

