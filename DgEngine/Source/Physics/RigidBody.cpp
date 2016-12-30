////////////////////////////////////////////////////////////////////////////////
// Filename: RigidBody.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "RigidBody.h"
#include "CharacterController.h"
#include "Physics.h"
#include "..\Transform.h"

////////////////
// NAMESPACES //
////////////////
using namespace glm;
using namespace dg;


RigidBody::RigidBody() {
	m_Actor = nullptr;
	m_CharacterController = false;
	SetMass(1.0f);
	SetDrag(1.0f);
	SetAngularDrag(0.05f);
	SetGravity(true);
	SetKinematic(false);
	SetCollisionDetection(eCollisionDetection::DISCRETE);
	FreezePositionX(false);
	FreezePositionY(false);
	FreezePositionZ(false);
	FreezeRotationX(false);
	FreezeRotationY(false);
	FreezeRotationZ(false);
}

RigidBody::~RigidBody() {
	if(m_Actor && !m_CharacterController) {
		PhysX::GetActiveScene()->removeActor(*m_Actor);
		m_Actor->release();
	}
}

void RigidBody::Initialize(SceneObject* sceneObject, bool isStatic = false) {
	Transform* transfom = GetComponentType(sceneObject, Transform);
	PxVec3 position(transfom->GetPosition()[0], 
		transfom->GetPosition()[1], 
		transfom->GetPosition()[2]);
	PxQuat rotation(transfom->GetRotation()[0],
		transfom->GetRotation()[1], 
		transfom->GetRotation()[2], 
		transfom->GetRotation()[3]);
	PxTransform pose(position, rotation);

	//if(physicsComponent->hasMeshCollider()) {
	//	finalRotation = PxQuat::createIdentity();		
	//} else {
	//	finalRotation = rotationX*rotationY*rotationZ;
	//}
	//rotation = PxQuat::createIdentity();

	if(isStatic) {
		m_Actor = PhysX::GetPhysics()->createRigidStatic(pose);
	} else {
		m_Actor = PhysX::GetPhysics()->createRigidDynamic(pose);
		// Set All properties
		SetMass(m_Mass);
		SetDrag(m_Drag);
		SetAngularDrag(m_AngularDrag);
		SetGravity(m_Gravity);
		SetKinematic(m_Kinematic);
		SetCollisionDetection(m_CollisionDetection);
		// TODO
		FreezePositionX(m_FreezePosition[0]);
		FreezePositionY(m_FreezePosition[1]);
		FreezePositionZ(m_FreezePosition[2]);
		FreezeRotationX(m_FreezeRotation[0]);
		FreezeRotationY(m_FreezeRotation[1]);
		FreezeRotationZ(m_FreezeRotation[2]);	
	}

	m_Actor->userData = (void *) sceneObject;
}

PxActor* RigidBody::GetActor() {
	return m_Actor;
}

void RigidBody::SetMass(PxReal value) {
	m_Mass = value;
	if(m_Actor && m_Actor->isRigidDynamic()) {
		PxRigidDynamic* rigidDynamicActor = (PxRigidDynamic*) m_Actor;
		rigidDynamicActor->setMass(value);

		if(m_Actor->isRigidBody()) {
			PxRigidBody* rigidBody = (PxRigidBody*) m_Actor;
			PxRigidBodyExt::updateMassAndInertia(*rigidBody, m_Mass);
		}
	}
}

void RigidBody::SetDrag(PxReal value) {
	m_Drag = value;
	if(m_Actor && m_Actor->isRigidDynamic()) {
		PxRigidDynamic* rigidDynamicActor = (PxRigidDynamic*) m_Actor;
		rigidDynamicActor->setLinearDamping(value);
	}
}

void RigidBody::SetAngularDrag(PxReal value) {
	m_AngularDrag = value;
	if(m_Actor && m_Actor->isRigidDynamic()) {
		PxRigidDynamic* rigidDynamicActor = (PxRigidDynamic*) m_Actor;
		rigidDynamicActor->setAngularDamping(value);
	}
}

void RigidBody::SetGravity(bool value) {
	m_Gravity = value;
	if(m_Actor && m_Actor->isRigidDynamic()) {
		PxRigidDynamic* rigidDynamicActor = (PxRigidDynamic*) m_Actor;
		rigidDynamicActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !value);
	}
}

void RigidBody::SetKinematic(bool value) {
	m_Kinematic = value;
	if(m_Actor && m_Actor->isRigidDynamic()) {
		PxRigidDynamic* rigidDynamicActor = (PxRigidDynamic*) m_Actor;
		rigidDynamicActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, value);
	}
}

void RigidBody::SetCollisionDetection(eCollisionDetection value) {
	m_CollisionDetection = value;
	if(m_Actor && m_Actor->isRigidDynamic()) {
		PxRigidDynamic* rigidDynamicActor = (PxRigidDynamic*) m_Actor;
	}
}

void RigidBody::FreezePositionX(bool value) {
	m_FreezePosition[0] = value;
	if(m_Actor && m_Actor->isRigidDynamic()) {
		PxRigidDynamic* rigidDynamicActor = (PxRigidDynamic*) m_Actor;
	}
}

void RigidBody::FreezePositionY(bool value) {
	m_FreezePosition[1] = value;
	if(m_Actor && m_Actor->isRigidDynamic()) {
		PxRigidDynamic* rigidDynamicActor = (PxRigidDynamic*) m_Actor;
	}
}

void RigidBody::FreezePositionZ(bool value) {
	m_FreezePosition[2] = value;
	if(m_Actor && m_Actor->isRigidDynamic()) {
		PxRigidDynamic* rigidDynamicActor = (PxRigidDynamic*) m_Actor;
	}
}

void RigidBody::FreezeRotationX(bool value) {
	m_FreezeRotation[0] = value;
	if(m_Actor && m_Actor->isRigidDynamic()) {
		PxRigidDynamic* rigidDynamicActor = (PxRigidDynamic*) m_Actor;
	}
}

void RigidBody::FreezeRotationY(bool value) {
	m_FreezeRotation[1] = value;
	if(m_Actor && m_Actor->isRigidDynamic()) {
		PxRigidDynamic* rigidDynamicActor = (PxRigidDynamic*) m_Actor;
	}
}

void RigidBody::FreezeRotationZ(bool value) {
	m_FreezeRotation[2] = value;
	if(m_Actor && m_Actor->isRigidDynamic()) {
		PxRigidDynamic* rigidDynamicActor = (PxRigidDynamic*) m_Actor;
	}
}
