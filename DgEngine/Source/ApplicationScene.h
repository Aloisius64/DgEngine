////////////////////////////////////////////////////////////////////////////////
// Filename: ApplicationScene.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <string>
#include <vector>

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class ApplicationManager;

	class ApplicationScene {
		friend class ApplicationManager;
	public:
		ApplicationScene(string, string = "");
		virtual ~ApplicationScene();

		virtual bool Initialize();
		virtual void LoadBehaviourScripts();
		virtual bool Frame();
		virtual void Release();

		const string& SceneName();

	private:
		string	m_NameScene;
		string	m_XmlConfigurationFile;
	};
}
