////////////////////////////////////////////////////////////////////////////////
// Filename: CharacterController.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Collider.h"

////////////////
// NAMESPACES //
////////////////


namespace dg {
	//	This struct is an helper for initialize the character controller
	struct CharacterControllerData {
		PxControllerShapeType::Enum			shapeType;
		PxVec3								upDirection;
		PxF32  								slopeLimit;
		PxF32  								invisibleWallHeight;
		PxF32  								maxJumpHeight;
		PxF32  								contactOffset;
		PxF32  								stepOffset;
		PxF32 								density;
		PxF32  								scaleCoeff;
		PxF32  								volumeGrowth;
		PxControllerBehaviorCallback*		behaviorCallback;
		PxControllerNonWalkableMode::Enum	nonWalkableMode;
		PhysicsMaterial*					material;
		void*								userData;
		//Only for Box;
		PxF32  								halfHeight;
		PxF32 								halfSideExtent;
		PxF32  								halfForwardExtent;
		//Only for capsule
		PxF32  								radius;
		PxF32  								height;
		PxCapsuleClimbingMode::Enum			climbingMode;

		CharacterControllerData() {
			this->shapeType = PxControllerShapeType::eCAPSULE;
			this->upDirection = PxVec3(0.0f, 1.0f, 0.0f);
			this->slopeLimit = 0.5f;
			this->invisibleWallHeight = 0.0f;
			this->maxJumpHeight = 0.0f;
			this->contactOffset = 0.1f;
			this->stepOffset = 0.01f;
			this->density = 10.0f;
			this->scaleCoeff = 0.8f;
			this->volumeGrowth = 1.5f;
			this->behaviorCallback = nullptr;
			this->nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING;
			this->material = nullptr;
			this->userData = nullptr;
			//Only for Box;
			this->halfHeight = -1.0f;
			this->halfSideExtent = -0.5f;
			this->halfForwardExtent = -0.5f;
			//Only for capsule
			this->radius = -0.5f;
			this->height = -0.5f;
			this->climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
		}
	};

	class CharacterController {
	public:
		CharacterController();
		virtual ~CharacterController();

		virtual void Initialize(SceneObject*, CharacterControllerData*);

		PxController* GetController();
		PxF32 GetHeight(PxF32);
		void StartJump(PxF32);
		void StopJump();
		bool CanJump();

	private:
		PxController*		m_CharacterController;
		PhysicsMaterial*	m_Material;
		PxF32				m_V0;
		PxF32				m_JumpTime;
		bool				m_Jump;
	};
}
