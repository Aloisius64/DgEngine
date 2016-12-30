////////////////////////////////////////////////////////////////////////////////
// Filename: Collider.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "..\SceneObject.h"
#include "PhysicsMaterial.h"
#include "PhysXIncludeFiles.h"

////////////////
// NAMESPACES //
////////////////
using namespace physx;


namespace dg {
	const enum eGeometryDirection {
		X_AXIS = 0,
		Y_AXIS,
		Z_AXIS
	};

	struct ColliderData {
		PxActor*			parentActor;
		PhysicsMaterial*	material;
		PxShape*			shape;
		vec3				center;
		bool				trigger;
		PxReal				radius;
		eGeometryDirection	direction;
		string				meshPath;
		bool				convex;
		PxReal				height;
		vec3				sizeCollider;

		ColliderData() {
			this->parentActor = nullptr;
			this->material = nullptr;
			this->shape = nullptr;
			this->center = vec_zero;
			this->trigger = false;
			this->radius = 0.0f;
			this->direction = eGeometryDirection::Y_AXIS;
			this->meshPath = "";
			this->convex = false;
			this->height = 0.0f;
			this->sizeCollider = vec_one;
		}
	};

	class Collider {
	public:
		Collider();
		virtual ~Collider();

		virtual void Initialize(const ColliderData&);
		virtual string GetType() = 0;
		SceneObject* GetSceneObject();
		PxActor* GetActor();

	protected:
		PxActor*			m_ParentActor;
		PxShape*			m_Shape;
		PhysicsMaterial*	m_Material;
		vec3				m_Center;
		bool				m_Trigger;
	};
}
