////////////////////////////////////////////////////////////////////////////////
// Filename: PhysicsMaterial.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "PhysicsMaterial.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


PhysicsMaterial::PhysicsMaterial() {
	m_Material = nullptr;
	m_DirtyValue = false;
}

PhysicsMaterial::~PhysicsMaterial() {
	if(m_Material) {
		m_Material->release();
	}
}

void PhysicsMaterial::SetName(string name) {
	m_Name = name;
}

const string& PhysicsMaterial::GetName() const {
	return m_Name;
}

void PhysicsMaterial::SetDynamicFriction(PxReal value) {
	m_DynamicFriction = value;
	m_DirtyValue = true;
}

void PhysicsMaterial::SetStaticFriction(PxReal value) {
	m_StaticFriction = value;
	m_DirtyValue = true;
}

void PhysicsMaterial::SetRestitution(PxReal value) {
	m_Restitution = value;
	m_DirtyValue = true;
}

PxMaterial* PhysicsMaterial::GetMaterial() {
	if(!m_Material || m_DirtyValue) {
		if(m_Material) {
			m_Material->release();
		}
		m_Material = PhysX::GetPhysics()->createMaterial(m_StaticFriction, m_DynamicFriction, m_Restitution);
		m_DirtyValue = false;
	}
	return m_Material;
}


