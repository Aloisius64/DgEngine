////////////////////////////////////////////////////////////////////////////////
// Filename: Script.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <string>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "SceneObject.h"
#include "Physics\PhysXIncludeFiles.h"
#include "UI.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;
using namespace physx;

namespace dg {
	class Behaviour;

	class Script {
		friend class Behaviour;
	public:
		Script(const string& = "");
		virtual ~Script();

		void Enable(bool = true);
		void Disable();
		bool IsEnabled() const;
		const string& GetName() const;

		virtual void OnActivate();
		virtual void Update();
		virtual void PostUpdate();
		virtual void Release();
		virtual void DrawUI(TwBar*);

		virtual void OnTriggerStart(SceneObject*, const PxTriggerPair);
		virtual void OnTriggerEnd(SceneObject*, const PxTriggerPair);
		virtual void OnColliderStart(SceneObject*, const PxContactPair);
		virtual void OnColliderPersist(SceneObject*, const PxContactPair);
		virtual void OnColliderEnd(SceneObject*, const PxContactPair);
		virtual void OnShapeHit(const PxControllerShapeHit&);
		virtual void OnControllerHit(const PxControllersHit&);
		virtual void OnObstacleHit(const PxControllerObstacleHit&);

		void SetSceneObject(SceneObject*);

	protected:
		SceneObject* GetSceneObject();

	private:
		SceneObject*	m_SceneObject;
		string			m_Name;
		bool			m_Enabled;
		bool			m_Activated;
	};
}
