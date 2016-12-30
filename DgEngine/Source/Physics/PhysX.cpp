////////////////////////////////////////////////////////////////////////////////
// Filename: PhysX.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////
#ifdef _WIN32
#include <Windows.h>
#endif

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "PhysX.h"
#include "Collider.h"
#include "PhysicsMaterial.h"
#include "..\Behaviour.h"
#include "..\SceneObject.h"
#include "..\TimeClock.h"
#include "..\Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;
using namespace dg;


#ifdef _DEBUG
#pragma comment(lib, "PhysX3CharacterKinematicDEBUG_x86.lib")                                 
#pragma comment(lib, "PhysX3CommonDEBUG_x86.lib")                                             
#pragma comment(lib, "PhysX3CookingDEBUG_x86.lib")                                            
#pragma comment(lib, "PhysX3DEBUG_x86.lib")                                                   
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")                                             
#pragma comment(lib, "PhysX3VehicleDEBUG.lib")                                                
#pragma comment(lib, "PhysXProfileSDKDEBUG.lib")                                              
#pragma comment(lib, "PhysXVisualDebuggerSDKDEBUG.lib")                                       
#pragma comment(lib, "PxTaskDEBUG.lib")
#else
#pragma comment(lib, "PhysX3CharacterKinematic_x86.lib")                                      
#pragma comment(lib, "PhysX3Common_x86.lib")                                                  
#pragma comment(lib, "PhysX3Cooking_x86.lib")                                                 
#pragma comment(lib, "PhysX3Extensions.lib")                                                  
#pragma comment(lib, "PhysX3Vehicle.lib")                                                     
#pragma comment(lib, "PhysX3_x86.lib")                                                        
#pragma comment(lib, "PhysXProfileSDK.lib")                                                   
#pragma comment(lib, "PhysXVisualDebuggerSDK.lib")                                            
#pragma comment(lib, "PxTask.lib")
#endif

#include <vector> 
using namespace std;

#define PVD_HOST "127.0.0.1"
#define PVD_HOST_PORT 5425
#define DEFAULT_STEP_TIME 1.0f/60.0f

unique_ptr<PhysX> PhysX::m_Instance = nullptr;

#pragma region ContactDetection
PxFilterFlags contactReportFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize) {
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);

	// all initial and persisting reports for everything, with per-point data
	pairFlags = PxPairFlag::eRESOLVE_CONTACTS
		|PxPairFlag::eNOTIFY_TOUCH_FOUND
		//|PxPairFlag::eNOTIFY_TOUCH_PERSISTS
		|PxPairFlag::eNOTIFY_TOUCH_LOST
		|PxPairFlag::eNOTIFY_CONTACT_POINTS;
	return PxFilterFlag::eDEFAULT;
}

class ContactReportCallback : public PxSimulationEventCallback {
	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) { PX_UNUSED(constraints); PX_UNUSED(count); }
	void onWake(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
	void onSleep(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
	void onTrigger(PxTriggerPair* pairs, PxU32 count) { 
		//PX_UNUSED(pairs); 
		//PX_UNUSED(count); 
		SceneObject* object1 = nullptr;
		SceneObject* object2 = nullptr;
		Behaviour* behaviourComponentObject1 = nullptr;
		Behaviour* behaviourComponentObject2 = nullptr;

		for(PxU32 i = 0; i<count; i++) {
			if(pairs[i].status==PxPairFlag::eNOTIFY_TOUCH_FOUND) {
#pragma region StartContact
				if(pairs[i].triggerShape->userData) {
					Collider* collider = (Collider*) pairs[i].triggerShape->userData;
					if(collider) {
						object1 = collider->GetSceneObject();
					}
				}
				if(pairs[i].otherShape->userData) {
					Collider* collider = (Collider*) pairs[i].otherShape->userData;
					if(collider && collider->GetActor()->userData) {
						object2 = collider->GetSceneObject();
					}
				}

				if(object1 && object2) {
					behaviourComponentObject1 = GetComponentType(object1, Behaviour);
					behaviourComponentObject2 = GetComponentType(object2, Behaviour);
					behaviourComponentObject1->OnTriggerStart(object2, pairs[i]);
					behaviourComponentObject2->OnTriggerStart(object1, pairs[i]);
				}
#pragma endregion
			} else if(pairs[i].status==PxPairFlag::eNOTIFY_TOUCH_LOST) {
#pragma region LostContact
				if(pairs[i].triggerShape->userData) {
					Collider* collider = (Collider*) pairs[i].triggerShape->userData;
					if(collider) {
						object1 = collider->GetSceneObject();
					}
				}
				if(pairs[i].otherShape->userData) {
					Collider* collider = (Collider*) pairs[i].otherShape->userData;
					if(collider && collider->GetActor()->userData) {
						object2 = collider->GetSceneObject();
					}
				}

				if(object1 && object2) {
					behaviourComponentObject1 = GetComponentType(object1, Behaviour);
					behaviourComponentObject2 = GetComponentType(object2, Behaviour);
					behaviourComponentObject1->OnTriggerEnd(object2, pairs[i]);
					behaviourComponentObject2->OnTriggerEnd(object1, pairs[i]);
				}
#pragma endregion
			}
		}

	}
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {
		PX_UNUSED((pairHeader));		
		SceneObject* object1 = nullptr;
		SceneObject* object2 = nullptr;
		Behaviour* behaviourComponentObject1 = nullptr;
		Behaviour* behaviourComponentObject2 = nullptr;

		for(PxU32 i = 0; i<nbPairs; i++) {
			if(pairs[i].events.isSet(PxPairFlag::eNOTIFY_TOUCH_FOUND)) {
#pragma region StartContact
				if(pairs[i].shapes[0]->userData) {
					Collider* collider = (Collider*) pairs[i].shapes[0]->userData;
					if(collider) {
						object1 = collider->GetSceneObject();
					}
				}
				if(pairs[i].shapes[1]->userData) {
					Collider* collider = (Collider*) pairs[i].shapes[1]->userData;
					if(collider && collider->GetActor()->userData) {
						object2 = collider->GetSceneObject();
					}
				}

				if(object1 && object2) {
					behaviourComponentObject1 = GetComponentType(object1, Behaviour);
					behaviourComponentObject2 = GetComponentType(object2, Behaviour);
					behaviourComponentObject1->OnColliderStart(object2, pairs[i]);
					behaviourComponentObject2->OnColliderStart(object1, pairs[i]);
				}
#pragma endregion
			} else if(pairs[i].events.isSet(PxPairFlag::eNOTIFY_TOUCH_PERSISTS)) {
#pragma region PersistContact
				if(pairs[i].shapes[0]->userData) {
					Collider* collider = (Collider*) pairs[i].shapes[0]->userData;
					if(collider) {
						object1 = collider->GetSceneObject();
					}
				}
				if(pairs[i].shapes[1]->userData) {
					Collider* collider = (Collider*) pairs[i].shapes[1]->userData;
					if(collider && collider->GetActor()->userData) {
						object2 = collider->GetSceneObject();
					}
				}

				if(object1 && object2) {
					behaviourComponentObject1 = GetComponentType(object1, Behaviour);
					behaviourComponentObject2 = GetComponentType(object2, Behaviour);
					behaviourComponentObject1->OnColliderPersist(object2, pairs[i]);
					behaviourComponentObject2->OnColliderPersist(object1, pairs[i]);
				}
#pragma endregion
			} else if(pairs[i].events.isSet(PxPairFlag::eNOTIFY_TOUCH_LOST)) {
#pragma region LostContact
				if(pairs[i].shapes[0]->userData) {
					Collider* collider = (Collider*) pairs[i].shapes[0]->userData;
					if(collider) {
						object1 = collider->GetSceneObject();
					}
				}
				if(pairs[i].shapes[1]->userData) {
					Collider* collider = (Collider*) pairs[i].shapes[1]->userData;
					if(collider && collider->GetActor()->userData) {
						object2 = collider->GetSceneObject();
					}
				}

				if(object1 && object2) {
					behaviourComponentObject1 = GetComponentType(object1, Behaviour);
					behaviourComponentObject2 = GetComponentType(object2, Behaviour);
					behaviourComponentObject1->OnColliderEnd(object2, pairs[i]);
					behaviourComponentObject2->OnColliderEnd(object1, pairs[i]);
				}
#pragma endregion
			}
		}
	}
};
#pragma endregion

#pragma region CharacterCallback
class CharacterReportCallback : public PxUserControllerHitReport, public PxControllerBehaviorCallback {
public:
	CharacterReportCallback(SceneObject* sceneObject) : PxUserControllerHitReport(), PxControllerBehaviorCallback() {
		this->sceneObject = sceneObject;
	}

	// Implements PxUserControllerHitReport
	virtual void onShapeHit(const PxControllerShapeHit& hit) {
		Behaviour* behaviourComponent = GetComponentType(sceneObject, Behaviour);
		if(behaviourComponent) {
			behaviourComponent->OnShapeHit(hit);
		}		
	}
	virtual void onControllerHit(const PxControllersHit& hit) {
		Behaviour* behaviourComponent = GetComponentType(sceneObject, Behaviour);
		if(behaviourComponent) {
			behaviourComponent->OnControllerHit(hit);
		}
	}
	virtual void onObstacleHit(const PxControllerObstacleHit& hit) {
		Behaviour* behaviourComponent = GetComponentType(sceneObject, Behaviour);
		if(behaviourComponent) {
			behaviourComponent->OnObstacleHit(hit);
		}
	}

	// Implements PxControllerBehaviorCallback
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxShape& shape, const PxActor& actor) {
		Behaviour* behaviourComponent = GetComponentType(sceneObject, Behaviour);
		if(behaviourComponent) {
			return behaviourComponent->GetBehaviorFlags(shape, actor);
		}
		return PxControllerBehaviorFlags(0);
	}
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxController& controller) {
		Behaviour* behaviourComponent = GetComponentType(sceneObject, Behaviour);
		if(behaviourComponent) {
			return behaviourComponent->GetBehaviorFlags(controller);
		}
		return PxControllerBehaviorFlags(0);
	}
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxObstacle& obstacle) {
		Behaviour* behaviourComponent = GetComponentType(sceneObject, Behaviour);
		if(behaviourComponent) {
			return behaviourComponent->GetBehaviorFlags(obstacle);
		}
		return PxControllerBehaviorFlags(0);
	}

private:
	SceneObject* sceneObject;
};
static vector<CharacterReportCallback*> contactReportCallbacks;
#pragma endregion

PhysX::PhysX() {
	m_Foundation = nullptr;
	m_Physics = nullptr;
	m_Dispatcher = nullptr;
	m_Connection = nullptr;
	m_ActiveScene = nullptr;
	m_Cooking = nullptr;
	m_ContactReportCallback = nullptr;
	m_CudaContextManager = nullptr;

	m_ActiveSceneName = "";
	m_Accumulator = 0.0f;
	m_StepSize = 1.0f/60.0f;
}

PhysX::~PhysX() {}

const unique_ptr<PhysX>& PhysX::Instance() {
	if(!m_Instance) {
		m_Instance = unique_ptr<PhysX>(new PhysX());
		m_Instance->Initialize();
	}
	return m_Instance;
}

void PhysX::Release() {
	if(PhysX::IsPhysXInitialized()) {
		// Clean all materials
		for each (auto physXMaterial in PhysX::Instance()->m_MaterialMap) {
			FREE_PTR(physXMaterial.second);
		}
		PhysX::Instance()->m_MaterialMap.clear();

		// Clean all controllers
		for each (auto physXControllerManager in PhysX::Instance()->m_Managers) {
			physXControllerManager.second->purgeControllers();
			physXControllerManager.second->release();
		}
		PhysX::Instance()->m_Managers.clear();

		// Clean all scenes
		for each (auto physXScene in PhysX::Instance()->m_Scenes) {
			physXScene.second->release();
		}
		PhysX::Instance()->m_Scenes.clear();

		PhysX::Instance()->m_Dispatcher->release();
		if(PhysX::Instance()->m_Connection) {
			PhysX::Instance()->m_Connection->release();
		}
		PxCloseExtensions();

		PhysX::Instance()->m_Physics->release();
		PhysX::Instance()->m_Cooking->release();
		FREE_PTR(PhysX::Instance()->m_ContactReportCallback);

		for each (auto physXContactReportCallback in contactReportCallbacks) {
			FREE_PTR(physXContactReportCallback);
		}
		contactReportCallbacks.clear();

		if(PhysX::Instance()->m_CudaContextManager) {
			PhysX::Instance()->m_CudaContextManager->release();
		}

		PhysX::Instance()->m_ProfileZoneManager->release();

		PhysX::Instance()->m_Foundation->release();

		PhysX::m_Instance = nullptr;
	}
}

void PhysX::Initialize() {
#ifdef _DEBUG
	printf("PhysX Initializing...\n");
#endif
	PX_UNUSED(true);

	PhysX::m_Instance->m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);
	m_ProfileZoneManager = &PxProfileZoneManager::createProfileZoneManager(m_Foundation);
	m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, PxTolerancesScale(), true, m_ProfileZoneManager);

	if(m_Physics==NULL) {
		//cerr<<"Error creating PhysX3 device."<<endl;
		//cerr<<"Exiting..."<<endl;
		exit(1);
	}

	if(!PxInitExtensions(*m_Physics)) {
		//cerr<<"PxInitExtensions failed!"<<endl;
	}

	if(m_Physics->getPvdConnectionManager()) {
		m_Physics->getVisualDebugger()->setVisualizeConstraints(true);
		m_Physics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_CONTACTS, true);
		m_Physics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true);
		m_Connection = PxVisualDebuggerExt::createConnection(m_Physics->getPvdConnectionManager(), PVD_HOST, PVD_HOST_PORT, 100);
	}

	m_Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_Foundation, PxCookingParams(PxTolerancesScale()));

	m_ContactReportCallback = new ContactReportCallback();

	//	CUDA creation
#if PX_SUPPORT_GPU_PHYSX
	PxCudaContextManagerDesc cudaContextManagerDesc;
	cudaContextManagerDesc.interopMode = PxCudaInteropMode::OGL_INTEROP;
	m_CudaContextManager = PxCreateCudaContextManager(*m_Foundation, cudaContextManagerDesc, m_ProfileZoneManager);
	if(m_CudaContextManager) {
		if(!m_CudaContextManager->contextIsValid()) {
			m_CudaContextManager->release();
			m_CudaContextManager = nullptr;
		}
	}
#endif

	if(m_CudaContextManager) {
		//cerr<<"Failed to initialize CUDA..."<<endl;
	}

	// Add a default scene. Maybe it is better move this part to another part!
	PxSceneDesc sceneDesc = PhysX::GetDefaultSceneDescription();
	PxScene* newScene = PhysX::AddScene("DefaultScene", &sceneDesc);	//gScene = gPhysX->createScene(sceneDesc);	
	PhysX::SetActiveScene(newScene);
}

PxScene* PhysX::CreateScene(PxSceneDesc* sceneDescription) {
	return PhysX::Instance()->m_Physics->createScene(*sceneDescription);
}

void PhysX::AddScene(string name, PxScene* scene) {
	PxControllerManager* controllerManager = PxCreateControllerManager(*scene);
	PhysX::Instance()->m_Managers[name] = controllerManager;
	PhysX::Instance()->m_Scenes[name] = scene;
}

void PhysX::RemoveScene(PxScene* scene) {
	string nameScene = "";

	for(PhysicsSceneMap::iterator it = PhysX::Instance()->m_Scenes.begin();
		it!=PhysX::Instance()->m_Scenes.end(); it++) {
		if(it->second==scene) {
			nameScene = it->first;
		}
	}

	if(nameScene!="") {
		PhysX::Instance()->m_Scenes[nameScene] = nullptr;
		scene->release();
		PxControllerManager* controllerManager = PhysX::Instance()->m_Managers[nameScene];
		PhysX::Instance()->m_Managers[nameScene] = nullptr;
		controllerManager->release();
	}
}

PxScene* PhysX::AddScene(string name, PxSceneDesc* sceneDescription) {
	PxScene* scene = PhysX::CreateScene(sceneDescription);
	PhysX::AddScene(name, scene);
	return scene;
}

void PhysX::SetActiveScene(PxScene* scene) {
	string nameScene = "";

	for(PhysicsSceneMap::iterator it = PhysX::Instance()->m_Scenes.begin();
		it!=PhysX::Instance()->m_Scenes.end(); it++) {
		if(it->second==scene) {
			nameScene = it->first;
		}
	}

	if(nameScene!="") {
		PhysX::SetActiveScene(nameScene);
	}
}

void PhysX::SetActiveScene(string name) {
	PhysX::Instance()->m_ActiveSceneName = name;
	PhysX::Instance()->m_ActiveScene = PhysX::Instance()->m_Scenes[name];
	PhysX::Instance()->m_ActiveManager = PhysX::Instance()->m_Managers[name];
}

string PhysX::GetNameActiveScene() {
	return PhysX::Instance()->m_ActiveSceneName;
}

PxScene* PhysX::GetActiveScene() {
	return PhysX::Instance()->m_ActiveScene;
}

//void PhysX::UpdatePhysics() {
//	if(PhysX::GetActiveScene()) {
//		PhysX::GetActiveScene()->simulate(DEFAULT_STEP_TIME);
//		PhysX::GetActiveScene()->fetchResults(true);
//	}
//}

void PhysX::UpdatePhysics() {
	if(!PhysX::IsPhysXInitialized())
		return;

	if(PhysX::GetActiveScene()) {
		PxReal deltaTime = TimeClock::Instance()->GetDeltaTime();
		PhysX::Instance()->m_Accumulator += deltaTime;
		if(PhysX::Instance()->m_Accumulator>=PhysX::Instance()->m_StepSize) {
			PhysX::Instance()->m_Accumulator -= PhysX::Instance()->m_StepSize;
			PhysX::GetActiveScene()->simulate(PhysX::Instance()->m_StepSize);
			PhysX::GetActiveScene()->fetchResults(true);
		}
	}
}

const PxSceneDesc PhysX::GetDefaultSceneDescription() {

	//	Detecting the physical processors number
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	PxU32 numProcessors = static_cast<int>(sysinfo.dwNumberOfProcessors > 1 ? sysinfo.dwNumberOfProcessors : 1);
	//std::cout<<"Detected "<<numProcessors<<" processors."<<endl;

	PxSceneDesc sceneDesc(PhysX::Instance()->m_Physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	PhysX::Instance()->m_Dispatcher = PxDefaultCpuDispatcherCreate(numProcessors);
	sceneDesc.cpuDispatcher = PhysX::Instance()->m_Dispatcher;
	sceneDesc.filterShader = contactReportFilterShader;
	//sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	sceneDesc.simulationEventCallback = PhysX::Instance()->m_ContactReportCallback;
	
	//sceneDesc.flags |= PxSceneFlag::eREQUIRE_RW_LOCK;

	return sceneDesc;
}

PxPhysics* PhysX::GetPhysics() {
	return Instance()->m_Physics;
}

PxCooking* PhysX::GetCooking() {
	return Instance()->m_Cooking;
}

PhysicsMaterial* PhysX::AddMaterialToMap(PhysicsMaterial* physicsMaterial) {
	if(!Instance()->m_MaterialMap[physicsMaterial->GetName()]) {
		Instance()->m_MaterialMap[physicsMaterial->GetName()] = physicsMaterial;
	}
	return Instance()->m_MaterialMap[physicsMaterial->GetName()];
}

PhysicsMaterial* PhysX::GetMaterial(const string& materialName) {
	PhysicsMaterialMap::const_iterator item = Instance()->m_MaterialMap.find(materialName);
	if(item!=Instance()->m_MaterialMap.end()) {
		return item->second;
	}
	return nullptr;
}

const PhysicsMaterialMap& PhysX::GetMaterialMap() {
	return Instance()->m_MaterialMap;
}

bool PhysX::IsPhysXInitialized() {
	return PhysX::m_Instance!=NULL;
}

PxController* PhysX::CreateController(PxControllerDesc* controllerDescription) {
	PxController* controller = nullptr;
	if(PhysX::Instance()->m_ActiveManager) {
		CharacterReportCallback* characterReportCallback = new CharacterReportCallback((SceneObject*)controllerDescription->userData);
		contactReportCallbacks.push_back(characterReportCallback);
		controllerDescription->reportCallback = characterReportCallback;
		controllerDescription->behaviorCallback = characterReportCallback;
		controller = PhysX::Instance()->m_ActiveManager->createController(*controllerDescription);
	} 
	//else {
	//	cerr<<"Error: Active PxControllerManager invalid!"<<endl;
	//}
	return controller;
}

PxVec3 PhysX::GetGravity() {
	return PhysX::Instance()->m_ActiveScene->getGravity();
}

PxDefaultAllocator* PhysX::GetAllocator() {
	return &Instance()->m_Allocator;
}

PxCudaContextManager* PhysX::GetCudaContextManager() {
	return Instance()->m_CudaContextManager;
}
