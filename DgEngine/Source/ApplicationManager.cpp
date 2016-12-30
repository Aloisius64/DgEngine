////////////////////////////////////////////////////////////////////////////////
// Filename: ApplicationManager.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////
#include <cstdlib>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "ApplicationManager.h"
#include "ApplicationScene.h"
#include "DataManager.h"
#include "LogoScene.h"
#include "SceneLoader.h"
#include "SceneObject.h"
#include "System.h"
#include "UI.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


#define APPLICATION_MANAGER_LOGO_SCENE "LogoScene"

unique_ptr<ApplicationManager> ApplicationManager::m_Instance = nullptr;

const unique_ptr<ApplicationManager>& ApplicationManager::Instance() {
	if(!m_Instance) {
		m_Instance = unique_ptr<ApplicationManager>(new ApplicationManager());

		// Set the LogoScene as first scene.
		auto logoScene = shared_ptr<ApplicationScene>(new LogoScene(APPLICATION_MANAGER_LOGO_SCENE));
		RegisterApplicationScene(logoScene);
		logoScene->Initialize();
		m_Instance->m_ActiveScene = logoScene;
	}

	return m_Instance;
}

ApplicationManager::ApplicationManager() {
	m_ActiveScene = nullptr;
	m_SceneToLoad = nullptr;
}

ApplicationManager::~ApplicationManager() {
	m_ActiveScene = nullptr;
	m_SceneToLoad = nullptr;
}

void ApplicationManager::Release() {
	if(m_Instance==nullptr)
		return;

	// Release all scenes.
	for(ApplicationSceneMap::iterator it = Instance()->m_ApplicationSceneMap.begin(); it!=Instance()->m_ApplicationSceneMap.end(); it++) {
		it->second->Release();
		it->second = nullptr;
	}
	Instance()->m_ApplicationSceneMap.clear();

	m_Instance->m_ActiveScene = nullptr;

	m_Instance = nullptr;
}

bool ApplicationManager::Frame() {
	static bool frame = false;

	// Render the active scene.
	frame = Instance()->m_ActiveScene->Frame();

	// Check if there is a new scene to load.
	if(Instance()->m_SceneToLoad) {
		Instance()->ChangeScene(Instance()->m_SceneToLoad);
		Instance()->m_SceneToLoad = nullptr;
	}

	return frame;
}

const shared_ptr<ApplicationScene>& ApplicationManager::CurrentScene() {
	return Instance()->m_ActiveScene;
}

void ApplicationManager::ChangeScene(shared_ptr<ApplicationScene>& newScene) {
	if(newScene) {
		//	Remove all the current nodes.
		SceneObject::CleanRoot();
		DataManager::Clean();
		SceneObject::InitRoot();

		if(newScene->Initialize()) {
			// Read scene file.
			SceneLoader::LoadScene(newScene->m_XmlConfigurationFile);

			// Load behaviour scripts.
			newScene->LoadBehaviourScripts();
			// Set the new scene as active scene.
			Instance()->m_ActiveScene = newScene;
			// UI class.
			UI::Instance()->InitializeSceneBar();
			UI::Instance()->InitializeEnvironmentBar();
			UI::Instance()->InitializeTexturesBar();
			UI::Instance()->InitializeMaterialsBar();
			// Set scene name as window title.
			System::Instance()->SetWindowTitle(newScene->m_NameScene.c_str());
		} else {
			newScene->Release();
			newScene = nullptr;
			Instance()->m_ActiveScene = Instance()->m_ApplicationSceneMap[APPLICATION_MANAGER_LOGO_SCENE];
		}
	}
}

void ApplicationManager::LoadScene(const string& nameScene) {
	if(Instance()->m_ApplicationSceneMap[nameScene]==nullptr)
		return;

	if(Instance()->m_ApplicationSceneMap[nameScene]==Instance()->m_ActiveScene)
		return;

	Instance()->m_SceneToLoad = Instance()->m_ApplicationSceneMap[nameScene];
}

void ApplicationManager::RegisterApplicationScene(ApplicationScene* applicationScene) {
	Instance()->m_ApplicationSceneMap.insert(ApplicationSceneMap::value_type(applicationScene->SceneName(), shared_ptr<ApplicationScene>(applicationScene)));
}

void ApplicationManager::RegisterApplicationScene(shared_ptr<ApplicationScene>& applicationScene) {
	Instance()->m_ApplicationSceneMap.insert(ApplicationSceneMap::value_type(applicationScene->SceneName(), applicationScene));
}

void ApplicationManager::UnregisterApplicationScene(ApplicationScene* ApplicationScene) {
	Instance()->m_ApplicationSceneMap.erase(ApplicationScene->SceneName());
}
