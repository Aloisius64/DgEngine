////////////////////////////////////////////////////////////////////////////////
// Filename: Behaviour.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <vector>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Component.h"
#include "Physics\PhysXIncludeFiles.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;
using namespace physx;

namespace dg {
	class Script;

	class Behaviour : public Component {

	public:
		static inline eComponentType ComponentType() {
			return eComponentType::BEHAVIOUR;
		}

	public:
		Behaviour();
		Behaviour(SceneObject*);
		virtual ~Behaviour();

		virtual void Update();
		virtual void PostUpdate();
		virtual void DrawUI(TwBar*);

		void OnTriggerStart(SceneObject*, const PxTriggerPair&);
		void OnTriggerEnd(SceneObject*, const PxTriggerPair&);
		void OnColliderStart(SceneObject*, const PxContactPair&);
		void OnColliderPersist(SceneObject*, const PxContactPair&);
		void OnColliderEnd(SceneObject*, const PxContactPair&);
		virtual void OnShapeHit(const PxControllerShapeHit&);
		virtual void OnControllerHit(const PxControllersHit&);
		virtual void OnObstacleHit(const PxControllerObstacleHit&);
		virtual PxControllerBehaviorFlags GetBehaviorFlags(const PxShape&, const PxActor&);
		virtual PxControllerBehaviorFlags GetBehaviorFlags(const PxController&);
		virtual PxControllerBehaviorFlags GetBehaviorFlags(const PxObstacle&);
		PxControllerBehaviorFlags(*GetBehaviorFlagsShapeActor)(const PxShape&, const PxActor&);
		PxControllerBehaviorFlags(*GetBehaviorFlagsController)(const PxController&);
		PxControllerBehaviorFlags(*GetBehaviorFlagsObstacle)(const PxObstacle&);

		void AddScript(Script*);
		void RemoveScript(Script*);
		const vector<Script*>& GetScripts() const;

	private:
		vector<Script*>		m_Scripts;
	};
}
