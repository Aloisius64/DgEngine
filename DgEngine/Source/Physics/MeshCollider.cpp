////////////////////////////////////////////////////////////////////////////////
// Filename: MeshCollider.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "MeshCollider.h"
#include "..\Mesh.h"
#include "..\DataManager.h"
#include "..\SceneObject.h"
#include "..\Transform.h"
#include "..\Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


MeshCollider::MeshCollider() : Collider() {
	m_MeshPath = "";
	m_Convex = false;
}

MeshCollider::~MeshCollider() {

}

string MeshCollider::GetType() {
	return "MeshCollider";
}

void MeshCollider::Initialize(SceneObject* sceneObject, const ColliderData& colliderData) {
	Collider::Initialize(colliderData);
	m_MeshPath = colliderData.meshPath;
	m_Convex = colliderData.convex;

	shared_ptr<Mesh> mesh = DataManager::GetMesh(m_MeshPath);
	Transform* transform = GetComponentType(sceneObject, Transform);

	//MeshGeometry, can be convex or not
	if(m_ParentActor->isRigidActor()) {
		PxRigidActor* rigidActor = (PxRigidStatic*) m_ParentActor;
		for(unsigned int i = 0; i<mesh->m_Entries.size(); i++) {
			//Count vertex for the current mesh
			PxU32 numVertex = 0;
			if((i+1)>=mesh->m_Entries.size()) {
				numVertex = mesh->m_Positions.size()-mesh->m_Entries[i].BaseVertex;
			} else {
				numVertex = mesh->m_Entries[i+1].BaseVertex-mesh->m_Entries[i].BaseVertex;
			}
			//Count indices for the current mesh
			PxU32 numIndices = 0;
			if((i+1)>=mesh->m_Entries.size()) {
				numIndices = mesh->m_Indices.size()-mesh->m_Entries[i].BaseIndex;
			} else {
				numIndices = mesh->m_Entries[i+1].BaseIndex-mesh->m_Entries[i].BaseIndex;
			}
			PxVec3* meshVerts = new PxVec3[numVertex];

			mat4 rotationMatrix = glm::toMat4(transform->GetRotation());

			mat4 scalingMatrix = glm::scale(mat4(1.0f), transform->GetScaling());

			for(unsigned int j = 0; j<numVertex; j++) {
				unsigned int positionIndex = j+mesh->m_Entries[i].BaseVertex;
				//First transform vertices
				vec4 vectorToTransform(mesh->m_Positions[positionIndex], 1.0f);
				vec4 result = vectorToTransform*(rotationMatrix*scalingMatrix);

				meshVerts[j] = PxVec3(result[0], result[1], result[2]);

				meshVerts[j] = PxVec3(mesh->m_Positions[positionIndex][0]*transform->GetScaling()[0],
					mesh->m_Positions[positionIndex][1]*transform->GetScaling()[1],
					mesh->m_Positions[positionIndex][2]*transform->GetScaling()[2]);
			}
			PxU32* indicesVerts = new PxU32[numIndices];
			for(unsigned int j = 0; j<numIndices; j++) {
				unsigned int indicesIndex = j+mesh->m_Entries[i].BaseIndex;
				indicesVerts[j] = PxU32(mesh->m_Indices[indicesIndex]);
			}
			//The vertices are back face!Conversion to front face
			//PxU32* indicesVertsInverted = new PxU32[numIndices];
			//PxU32* indicesVerts = new PxU32[numIndices];
			//for(unsigned int j = 0; j<numIndices; j++) {
			//	unsigned int indicesIndex = j+mesh->m_Entries[i].BaseIndex;
			//	indicesVertsInverted[j] = PxU32(mesh->m_Indices[indicesIndex]);
			//}
			//for(unsigned int j = 0; j<numIndices; j++) {
			//	unsigned int indicesIndex = j+mesh->m_Entries[i].BaseIndex;
			//	indicesVerts[j] = indicesVertsInverted[(numIndices-1)-j];
			//}
			if(m_Convex && numVertex<256) {
				PxConvexMeshDesc convexDesc;
				convexDesc.points.count = numVertex;
				convexDesc.points.stride = sizeof(PxVec3);
				convexDesc.points.data = meshVerts;
				convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
				PxDefaultMemoryOutputStream buf;
				PxConvexMeshCookingResult::Enum result;
				if(PhysX::GetCooking()->cookConvexMesh(convexDesc, buf, &result)) {
					PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
					PxConvexMesh* convexMesh = PhysX::GetPhysics()->createConvexMesh(input);
					PxConvexMeshGeometry convexMeshGeometry(convexMesh);
					if(convexMeshGeometry.isValid()) {
						m_Shape = PhysX::GetPhysics()->createShape(convexMeshGeometry, *m_Material->GetMaterial());

						m_Shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !m_Trigger);
						m_Shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, m_Trigger);

						rigidActor->attachShape(*m_Shape);
						m_Shape->userData = this;
					}
				}
			} else {
				PxTriangleMeshDesc meshDesc;
				meshDesc.points.count = numVertex;
				meshDesc.points.stride = sizeof(PxVec3);
				meshDesc.points.data = meshVerts;
				meshDesc.triangles.count = numIndices/3;
				meshDesc.triangles.stride = 3*sizeof(PxU32);
				meshDesc.triangles.data = indicesVerts;
				PxDefaultMemoryOutputStream writeBuffer;
				if(PhysX::GetCooking()->cookTriangleMesh(meshDesc, writeBuffer)) {
					PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
					PxTriangleMesh* triangleMesh = PhysX::GetPhysics()->createTriangleMesh(readBuffer);
					PxTriangleMeshGeometry triangleMeshGeometry = PxTriangleMeshGeometry(triangleMesh);
					if(triangleMeshGeometry.isValid()) {
						m_Shape = PhysX::GetPhysics()->createShape(triangleMeshGeometry, *m_Material->GetMaterial());

						m_Shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !m_Trigger);
						m_Shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, m_Trigger);

						rigidActor->attachShape(*m_Shape);
						m_Shape->userData = this;
					} else {
						dg::DebugConsoleMessage("Wrong mesh!\n");
					}
				}
			}
			delete meshVerts;
			delete indicesVerts;
		}
	}
}

/*----------------------------------------------------------------------------------*/

//PxVec3* meshVerts = new PxVec3[mesh->Positions.size()];
//for(unsigned int i = 0; i<mesh->Positions.size(); i++) {
//	meshVerts[i] = PxVec3(mesh->Positions[i].x*transform->getScaling().x,
//		mesh->Positions[i].y*transform->getScaling().y,
//		mesh->Positions[i].z*transform->getScaling().z);
//}
//PxU32* indicesVerts = new PxU32[mesh->Indices.size()];
//for(unsigned int i = 0; i<mesh->Indices.size(); i++) {
//	indicesVerts[i] = PxU32(mesh->Indices[i]);
//}
//PxTriangleMeshDesc meshDesc;
//meshDesc.points.count = mesh->Positions.size();
//meshDesc.points.stride = sizeof(PxVec3);
//meshDesc.points.data = meshVerts;
//meshDesc.triangles.count = mesh->Indices.size();
//meshDesc.triangles.stride = 3*sizeof(PxU32);
//meshDesc.triangles.data = indicesVerts;
//PxDefaultMemoryOutputStream writeBuffer;
//if(Physics::getCooking()->cookTriangleMesh(meshDesc, writeBuffer)) {
//	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
//	PxTriangleMesh* triangleMesh = Physics::getPhysics()->createTriangleMesh(readBuffer);
//	PxTriangleMeshGeometry triangleMeshGeometry = PxTriangleMeshGeometry(triangleMesh);
//	if(triangleMeshGeometry.isValid()) {
//		PxShape* shape = Physics::getPhysics()->createShape(triangleMeshGeometry, *material->getMaterial());
//		rigidActor->attachShape(*shape);
//		shape->release();
//	}
//}
//delete meshVerts;
//delete indicesVerts;
//
//for(unsigned int i = 0; i<mesh->m_Entries.size(); i++) {
//	// Count vertex for the current mesh
//	PxU32 numVertex = 0;
//	if((i+1)>=mesh->m_Entries.size()) {
//		numVertex = mesh->Positions.size()-mesh->m_Entries[i].BaseVertex;
//	} else {
//		numVertex = mesh->m_Entries[i+1].BaseVertex-mesh->m_Entries[i].BaseVertex;
//	}
//	// Count indices for the current mesh
//	PxU32 numIndices = 0;
//	if((i+1)>=mesh->m_Entries.size()) {
//		numIndices = mesh->Indices.size()-mesh->m_Entries[i].BaseIndex;
//	} else {
//		numIndices = mesh->m_Entries[i+1].BaseIndex-mesh->m_Entries[i].BaseIndex;
//	}
//
//	PxVec3* meshVerts = new PxVec3[numVertex];
//	for(unsigned int j = 0; j<numVertex; j++) {
//		unsigned int positionIndex = j+mesh->m_Entries[i].BaseVertex;
//		meshVerts[j] = PxVec3(mesh->Positions[positionIndex].x*transform->getScaling().x,
//			mesh->Positions[positionIndex].y*transform->getScaling().y,
//			mesh->Positions[positionIndex].z*transform->getScaling().z);
//	}
//	PxU32* indicesVerts = new PxU32[numIndices];
//	for(unsigned int j = 0; j<numIndices; j++) {
//		unsigned int indicesIndex = j+mesh->m_Entries[i].BaseIndex;
//		indicesVerts[j] = PxU32(mesh->Indices[indicesIndex]);
//	}
//
//	PxTriangleMeshDesc meshDesc;
//	meshDesc.points.count = numVertex;
//	meshDesc.points.stride = sizeof(PxVec3);
//	meshDesc.points.data = meshVerts;
//	meshDesc.triangles.count = numIndices/3;
//	meshDesc.triangles.stride = 3*sizeof(PxU32);
//	meshDesc.triangles.data = indicesVerts;
//
//	PxDefaultMemoryOutputStream writeBuffer;
//	if(Physics::getCooking()->cookTriangleMesh(meshDesc, writeBuffer)) {
//		PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
//		PxTriangleMesh* triangleMesh = Physics::getPhysics()->createTriangleMesh(readBuffer);
//		PxTriangleMeshGeometry triangleMeshGeometry = PxTriangleMeshGeometry(triangleMesh);
//		if(triangleMeshGeometry.isValid()) {
//			PxShape* shape = Physics::getPhysics()->createShape(triangleMeshGeometry, *material->getMaterial());
//			rigidActor->attachShape(*shape);
//			shape->release();
//		}
//	}
//
//	delete meshVerts;
//	delete indicesVerts;
//}
