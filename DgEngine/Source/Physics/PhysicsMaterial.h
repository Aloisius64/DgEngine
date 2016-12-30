////////////////////////////////////////////////////////////////////////////////
// Filename: PhysicsMaterial.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <string>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "PhysX.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;
using namespace physx;


namespace dg {
	class PhysicsMaterial {
	public:
		PhysicsMaterial();
		~PhysicsMaterial();

		void SetName(string);
		const string& GetName() const;
		void SetDynamicFriction(PxReal);
		void SetStaticFriction(PxReal);
		void SetRestitution(PxReal);
		PxMaterial* GetMaterial();

	private:
		PxMaterial*		m_Material;
		string			m_Name;
		PxReal			m_DynamicFriction;
		PxReal			m_StaticFriction;
		PxReal			m_Restitution;
		bool			m_DirtyValue;
	};
}
