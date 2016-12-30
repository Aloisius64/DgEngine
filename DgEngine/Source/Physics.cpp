////////////////////////////////////////////////////////////////////////////////
// Filename: Physics.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Physics.h"
#include "Gizmo.h"
#include "Physics\MeshCollider.h"
#include "System.h"
#include "TimeClock.h"
#include "Transform.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


Physics::Physics(SceneObject* sceneObject, RigidBody* rigidBody)
	: Component(eComponentType::PHYSICS, sceneObject) {
	m_RigidBody = rigidBody;
	m_CharacterController = nullptr;
	m_MovementVector = PxVec3(0.0f, 0.0f, 0.0f);
}

Physics::~Physics() {
	for(unsigned int i = 0; i<m_Colliders.size(); i++) {
		delete m_Colliders[i];
	}
	m_Colliders.clear();

	FREE_PTR(m_CharacterController);
	FREE_PTR(m_RigidBody);
}

void Physics::Update() {
	if (!IsEnabled())
		return;

	Transform* transform = GetComponentType(GetSceneObject(), Transform);
	PxRigidActor* rigidActor = (PxRigidActor*) m_RigidBody->GetActor();
	if(m_CharacterController) {
		float deltaTime = TimeClock::Instance()->GetDeltaTime();

		PxVec3 gravity = PhysX::GetGravity();

		const PxF32 heightDelta = m_CharacterController->GetHeight(deltaTime);
		PxVec3 deltaVector(0.0f, 0.0f, 0.0f);
		PxVec3 upDirection = m_CharacterController->GetController()->getUpDirection();
		upDirection.normalize();
		float dy = 0.0f;
		if(heightDelta!=0.0f) {
			deltaVector.x = heightDelta*upDirection.x;
			deltaVector.y = heightDelta*upDirection.y;
			deltaVector.z = heightDelta*upDirection.z;
			dy = heightDelta;
		} else {
			deltaVector.x = gravity.x * deltaTime;
			deltaVector.y = gravity.y * deltaTime;
			deltaVector.z = gravity.z * deltaTime;
			dy = gravity.y * deltaTime;
		}

		PxVec3 displacement = m_MovementVector+deltaVector;
		displacement.y = deltaVector.y;
		
		const PxU32 flags = m_CharacterController->GetController()->move(displacement, 0.001f, deltaTime, NULL, NULL);
		if(flags & PxControllerCollisionFlag::eCOLLISION_DOWN) {
			m_CharacterController->StopJump();
		}

		m_MovementVector = PxVec3(0.0f, 0.0f, 0.0f);

		PxTransform pose = rigidActor->getGlobalPose();
		transform->SetPosition(pose.p.x, pose.p.y, pose.p.z);
		//transform->SetRotation(quat(pose.q.w, pose.q.x, pose.q.y, pose.q.z));
	} else if(rigidActor) {
		PxTransform pose = rigidActor->getGlobalPose();
		transform->SetPosition(pose.p.x, pose.p.y, pose.p.z);
		transform->SetRotation(quat(pose.q.w, pose.q.x, pose.q.y, pose.q.z));
	}
}

void Physics::DrawUI(TwBar* bar) {
	char parameter[128];
	memset(parameter, 0, sizeof(parameter));

	TwAddButton(bar, "Physics", NULL, NULL, "");
	TwAddVarCB(bar, "Physics_Enabled", TW_TYPE_BOOL8, EnabledCallback, IsEnabledCallback, this, " label='Enabled' ");
	TwAddSeparator(bar, "", NULL);
	TwAddSeparator(bar, "", NULL);
}

RigidBody* Physics::GetRigidBody() {
	return m_RigidBody;
}

void Physics::AddCollider(Collider* collider) {
	m_Colliders.push_back(collider);
}

void Physics::Move(const vec3& movement) {
	if(m_CharacterController) {
		m_MovementVector.x = movement[0];
		m_MovementVector.y = movement[1];
		m_MovementVector.z = movement[2];
	}
}

void Physics::Jump(float force) {
	if(m_CharacterController->CanJump()) {
		m_CharacterController->StartJump(force);
	}
}

bool Physics::HasMeshCollider() {
	for(vector<Collider*>::iterator it = m_Colliders.begin(); it!=m_Colliders.end(); it++) {
		MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(*it);
		if(meshCollider) {
			return true;
		}
	}
	return false;
}

//if(characterController) {
//	float deltaTime = DgApplicationManager::getInstance()->GetDeltaTime();
//
//	PxVec3 gravity = Physics::getGravity();
//	gravity.normalize();
//
//	const PxF32 heightDelta = mCCTs[i]->mJump.getHeight(dtime);
//	float dy;
//	if(heightDelta!=0.0f)
//		dy = heightDelta;
//	else
//		dy = mGravity * dtime;
//
//
//	PxVec3 displacement = gravity+m_movementVector;
//	const PxU32 flags = characterController->getController()->move(displacement, 0.001f, deltaTime, NULL, NULL);
//
//	// TODO: Create a reset function
//	m_movementVector.x = 0.0f;
//	m_movementVector.y = 0.0f;
//	m_movementVector.z = 0.0f;
//
//	PxTransform pose = rigidActor->getGlobalPose();
//	//	Update position
//	transform->setPosition(pose.p.x, pose.p.y, pose.p.z);
//}
