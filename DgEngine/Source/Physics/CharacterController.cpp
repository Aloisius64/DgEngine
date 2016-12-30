////////////////////////////////////////////////////////////////////////////////
// Filename: CharacterController.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "CharacterController.h"
#include "Physics.h"
#include "RigidBody.h"
#include "..\Mesh.h"
#include "..\DataManager.h"
#include "..\MeshRenderer.h"
#include "..\Transform.h"
#include "..\Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


static PxF32 gJumpGravity = -50.0f;

CharacterController::CharacterController()
	: m_V0(0.0f),
	m_JumpTime(0.0f),
	m_Jump(false) {
	m_CharacterController = nullptr;
}

CharacterController::~CharacterController() {
	if(m_CharacterController) {
		m_CharacterController->release();
	}
	m_Material = nullptr;
}

void CharacterController::Initialize(SceneObject* sceneObject, CharacterControllerData* characterControllerData) {
	PxControllerDesc* characterDesc;
	PxBoxControllerDesc boxDesc;
	PxCapsuleControllerDesc capsuleDesc;
	bool calculateDimension = false;
	Transform* transform = GetComponentType(sceneObject, Transform);

	if(characterControllerData->shapeType==PxControllerShapeType::eBOX) {
		calculateDimension = (characterControllerData->halfHeight<0||
			characterControllerData->halfSideExtent<0||
			characterControllerData->halfForwardExtent<0);
	} else {
		calculateDimension = (characterControllerData->height<0||
			characterControllerData->radius<0);
	}
	
	//	Auto set character controller dimensions
	if(calculateDimension) {
		MeshRenderer* meshRenderer = GetComponentType(sceneObject, MeshRenderer);
		shared_ptr<Mesh> mesh = meshRenderer->GetMesh();
		vec3 minValues = mesh->GetMinExtension();
		vec3 maxValues = mesh->GetMaxExtension();
		minValues[0] *= transform->GetScaling()[0];
		minValues[1] *= transform->GetScaling()[1];
		minValues[2] *= transform->GetScaling()[2];
		maxValues[0] *= transform->GetScaling()[0];
		maxValues[1] *= transform->GetScaling()[1];
		maxValues[2] *= transform->GetScaling()[2];
		characterControllerData->halfHeight = (maxValues[1]-minValues[1])/2.0f;
		characterControllerData->halfSideExtent = (maxValues[0]-minValues[0])/2.0f;
		characterControllerData->halfForwardExtent = (maxValues[2]-minValues[2])/2.0f;
		characterControllerData->height = maxValues[1]-minValues[1];
		characterControllerData->radius = glm::max(maxValues[0]-minValues[0], maxValues[2]-minValues[2]);
	}

	if(characterControllerData->shapeType==PxControllerShapeType::eBOX) {
		boxDesc.halfHeight = characterControllerData->halfHeight;
		boxDesc.halfSideExtent = characterControllerData->halfSideExtent;
		boxDesc.halfForwardExtent = characterControllerData->halfForwardExtent;
		characterDesc = &boxDesc;
	} else {
		capsuleDesc.height = characterControllerData->height;
		capsuleDesc.radius = characterControllerData->radius;
		capsuleDesc.climbingMode = characterControllerData->climbingMode;
		characterDesc = &capsuleDesc;
	}
	characterDesc->position = PxExtendedVec3(transform->GetPosition()[0], transform->GetPosition()[1], transform->GetPosition()[2]);
	characterDesc->upDirection = characterControllerData->upDirection;
	characterDesc->slopeLimit = characterControllerData->slopeLimit;
	characterDesc->invisibleWallHeight = characterControllerData->invisibleWallHeight;
	characterDesc->maxJumpHeight = characterControllerData->maxJumpHeight;
	characterDesc->contactOffset = characterControllerData->contactOffset;
	characterDesc->stepOffset = characterControllerData->stepOffset;
	//characterDesc->density = characterControllerData->density;
	//characterDesc->scaleCoeff = characterControllerData->scaleCoeff;
	//characterDesc->volumeGrowth = characterControllerData->volumeGrowth;
	//characterDesc->behaviorCallback = characterControllerData->behaviorCallback;
	characterDesc->nonWalkableMode = characterControllerData->nonWalkableMode;
	characterDesc->material = characterControllerData->material->GetMaterial();
	characterDesc->userData = characterControllerData->userData;

	m_CharacterController = PhysX::CreateController(characterDesc);

	// Create DgRigidBody for the Physics component
	if(m_CharacterController) {
		PxRigidDynamic* actor = m_CharacterController->getActor();
		actor->userData = sceneObject;
		Physics* physicsComponent = GetComponentType(sceneObject, Physics);
		if(physicsComponent) {
			RigidBody* rigidBody = physicsComponent->GetRigidBody();
			rigidBody->m_Actor = actor;
			rigidBody->m_CharacterController = true;
		}
		m_Material = characterControllerData->material;
	}
}

PxController* CharacterController::GetController() {
	return m_CharacterController;
}

void CharacterController::StartJump(PxF32 v0) {
	if(m_Jump)	return;
	m_JumpTime = 0.0f;
	m_V0 = v0;
	m_Jump = true;
}

void CharacterController::StopJump() {
	if(!m_Jump)	return;
	m_Jump = false;
	//mJumpTime = 0.0f;
	//mV0	= 0.0f;
}

PxF32 CharacterController::GetHeight(PxF32 elapsedTime) {
	if(!m_Jump)	return 0.0f;
	m_JumpTime += elapsedTime;
	const PxF32 h = gJumpGravity*m_JumpTime*m_JumpTime+m_V0*m_JumpTime;
	return h*elapsedTime;
}

bool CharacterController::CanJump() {
	PxControllerState cctState;
	m_CharacterController->getState(cctState);
	return (cctState.collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN)!=0;
}
