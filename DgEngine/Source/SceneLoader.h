////////////////////////////////////////////////////////////////////////////////
// Filename: SceneLoader.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <memory>
#include <string>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "SceneObject.h"
#include "Physics\PhysX.h"
#include "Physics\RigidBody.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class Material;

	class SceneLoader {
	public:
		static void	LoadScene(string);

	private:
		static void	ParseGraphScene(SceneObject*);
		static void	ParseEnvironment();
		static void	ParseSceneObject(SceneObject*);
		static void	ParseComponents(SceneObject*);
		static void	ParseTransform(SceneObject*);
		static void	ParseCamera(SceneObject*);
		static void	ParseMesh(SceneObject*);
		static void ParseMaterials(vector<shared_ptr<Material>>& materials);
		static shared_ptr<Material> ParseMaterial();
		static void ParseLight(SceneObject*);
		static void ParseBehaviour(SceneObject*);
		static void ParsePhysics(SceneObject*);
		static void ParseColliders(SceneObject*, RigidBody*);
		static PhysicsMaterial* ParsePhysicsMaterial();
		static void ParsePlaneCollider(SceneObject*, RigidBody*);
		static void ParseBoxCollider(SceneObject*, RigidBody*);
		static void ParseSphereCollider(SceneObject*, RigidBody*);
		static void ParseCapsuleCollider(SceneObject*, RigidBody*);
		static void ParseMeshCollider(SceneObject*, RigidBody*);
		static void ParseTerrainCollider(SceneObject*, RigidBody*);
		static void ParseCharacterController(SceneObject*);
		//static void ParseAudio(SceneObject*);
		//static void ParseParticles(SceneObject*);
	};
}