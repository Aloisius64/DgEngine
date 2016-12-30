////////////////////////////////////////////////////////////////////////////////
// Filename: ApplicationManager.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <map>
#include <memory>
#include <string>

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class ApplicationScene;

	typedef map<string, shared_ptr<ApplicationScene>, less<string>> ApplicationSceneMap;

	class ApplicationManager {
	public:
		static const unique_ptr<ApplicationManager>& Instance();
		static const shared_ptr<ApplicationScene>& CurrentScene();
		static bool Frame();
		static void LoadScene(const string&);
		static void ChangeScene(shared_ptr<ApplicationScene>&);
		static void RegisterApplicationScene(ApplicationScene*);
		static void RegisterApplicationScene(shared_ptr<ApplicationScene>&);
		static void UnregisterApplicationScene(ApplicationScene*);
		static void Release();

	public:
		virtual ~ApplicationManager();

	private:
		ApplicationManager();

	private:
		static unique_ptr<ApplicationManager>	m_Instance;
		shared_ptr<ApplicationScene>			m_ActiveScene;
		shared_ptr<ApplicationScene>			m_SceneToLoad;
		ApplicationSceneMap						m_ApplicationSceneMap;
	};
}
