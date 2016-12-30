////////////////////////////////////////////////////////////////////////////////
// Filename: Physics.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Component.h"
#include "Mathematics.h"
#include "Physics\RigidBody.h"
#include "Physics\CharacterController.h"

////////////////
// NAMESPACES //
////////////////


namespace dg {
	class SceneLoader;

	class Physics : public Component {
		friend class SceneLoader;

	public:
		static inline eComponentType ComponentType() {
			return eComponentType::PHYSICS;
		}

	public:
		Physics(SceneObject*, RigidBody*);
		virtual ~Physics();

		virtual void Update();
		virtual void DrawUI(TwBar*);

		RigidBody* GetRigidBody();
		void AddCollider(Collider* collider);

		void Move(const vec3& movement);
		void Jump(float force);

		bool HasMeshCollider();

	private:
		RigidBody*				m_RigidBody;
		vector<Collider*>		m_Colliders;
		CharacterController*	m_CharacterController;
		PxVec3					m_MovementVector;
	};
}
