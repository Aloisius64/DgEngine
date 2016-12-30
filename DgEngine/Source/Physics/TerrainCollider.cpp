////////////////////////////////////////////////////////////////////////////////
// Filename: TerrainCollider.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "TerrainCollider.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


TerrainCollider::TerrainCollider() : Collider() {

}

TerrainCollider::~TerrainCollider() {

}

string TerrainCollider::GetType() {
	return "TerrainCollider";
}

void TerrainCollider::Initialize(const ColliderData& colliderData) {
	Collider::Initialize(colliderData);
	
	// TerrainGeometry
	if(m_ParentActor->isRigidActor()) {
		//PxRigidActor* rigidActor = (PxRigidStatic*) m_ParentActor;
		//shape = Physics::getPhysics()->createShape(PxBoxGeometry(this->sizeCollider.x, this->sizeCollider.y, this->sizeCollider.z), *material->getMaterial());

		//if(trigger) {
		//	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		//	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
		//}

		//rigidDynamicActor->attachShape(*shape);
		//shape->userData = this;
	}
}

