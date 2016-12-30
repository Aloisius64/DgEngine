////////////////////////////////////////////////////////////////////////////////
// Filename: PhysX.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <map>
#include <memory>
#include <string>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "PxPhysicsAPI.h"
#include "PhysXIncludeFiles.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;
using namespace physx;


namespace dg {
	class PhysicsMaterial;

	typedef map<string, PxScene*, less<string>>				PhysicsSceneMap;
	typedef map<string, PxControllerManager*, less<string>> PhysicsControllerManagerMap;
	typedef map<string, PhysicsMaterial*, less<string>>		PhysicsMaterialMap;

	class PhysX {
	public:
		~PhysX();

		static const unique_ptr<PhysX>& Instance();
		static void Release();
		static PxScene* CreateScene(PxSceneDesc* sceneDescription);
		static void AddScene(string name, PxScene* scene);
		static void RemoveScene(PxScene* scene);
		static PxScene* AddScene(string name, PxSceneDesc* sceneDescription);
		static void SetActiveScene(PxScene* scene);
		static void SetActiveScene(string name);
		static string GetNameActiveScene();
		static PxScene* GetActiveScene();
		static void UpdatePhysics();
		static const PxSceneDesc GetDefaultSceneDescription();
		static PxPhysics* GetPhysics();
		static PxCooking* GetCooking();

		static PhysicsMaterial* AddMaterialToMap(PhysicsMaterial*);
		static PhysicsMaterial* GetMaterial(const string&);
		static const PhysicsMaterialMap& GetMaterialMap();

		static bool IsPhysXInitialized();
		static PxController* CreateController(PxControllerDesc* controllerDescription);
		static PxVec3 GetGravity();
		static PxDefaultAllocator* GetAllocator();
		static PxCudaContextManager* GetCudaContextManager();

	private:
		PhysX();
		void Initialize();

	private:
		static unique_ptr<PhysX>	m_Instance;
		PxDefaultAllocator			m_Allocator;
		PxDefaultErrorCallback		m_ErrorCallback;
		PxFoundation*				m_Foundation;
		PxPhysics*					m_Physics;
		PxDefaultCpuDispatcher*		m_Dispatcher;
		PxCooking*					m_Cooking;
		PxVisualDebuggerConnection* m_Connection;
		PxProfileZoneManager*		m_ProfileZoneManager;
		PxSimulationEventCallback*	m_ContactReportCallback;
		PxCudaContextManager*		m_CudaContextManager;

		string						m_ActiveSceneName;
		PxScene*					m_ActiveScene;
		PhysicsSceneMap				m_Scenes;
		PxControllerManager*		m_ActiveManager;
		PhysicsControllerManagerMap m_Managers;

		//	Variables utilized in update
		PxReal						m_Accumulator;
		PxReal						m_StepSize;

		//	Memory optimization for material
		PhysicsMaterialMap			m_MaterialMap;
	};
}
