////////////////////////////////////////////////////////////////////////////////
// Filename: RigidBody.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <vector>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "..\SceneObject.h"
#include "PhysX.h"
#include "Collider.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;
using namespace physx;


namespace dg {
	class CharacterController;

	const enum eCollisionDetection {
		DISCRETE = 0,
		CONTINUOUS,
		CONTINOUOUS_DYNAMIC,
		NO_COLLISION_DETECTION
	};

	class RigidBody {
		friend class CharacterController;
	public:
		RigidBody();
		~RigidBody();

		void Initialize(SceneObject*, bool);
		PxActor* GetActor();
		void SetMass(PxReal);
		void SetDrag(PxReal);
		void SetAngularDrag(PxReal);
		void SetGravity(bool);
		void SetKinematic(bool);
		void SetCollisionDetection(eCollisionDetection);
		void FreezePositionX(bool);
		void FreezePositionY(bool);
		void FreezePositionZ(bool);
		void FreezeRotationX(bool);
		void FreezeRotationY(bool);
		void FreezeRotationZ(bool);

	private:
		PxActor*			m_Actor;
		bool				m_CharacterController;
		PxReal 				m_Mass;
		PxReal 				m_Drag;
		PxReal 				m_AngularDrag;
		bool 				m_Gravity;
		bool 				m_Kinematic;
		eCollisionDetection m_CollisionDetection;
		bvec3				m_FreezePosition;
		bvec3				m_FreezeRotation;
	};
}
