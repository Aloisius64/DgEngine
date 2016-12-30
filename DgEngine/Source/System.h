////////////////////////////////////////////////////////////////////////////////
// Filename: System.h
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
#include "ApplicationManager.h"
#include "Mathematics.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	enum eGraphicAPI {
		OPEN_GL = 0,
		//DIRECT_X
		//OPEN_GL_ES
	};

	class System {
	public:
		static const unique_ptr<System>& Instance(enum eGraphicAPI = eGraphicAPI::OPEN_GL);

	public:
		virtual ~System();

		virtual bool Initialize() = 0;
		virtual bool EndRun() = 0;
		virtual void Shutdown();
		virtual void SwapBuffers() const;
		virtual void ClearBuffers(glm::vec4) const;
		virtual void PollEvents() const;
		virtual void UpdateVieport(int, int, int, int) const;
		virtual void ShowCursor() = 0;
		virtual void HideCursor() = 0;
		virtual void SetWindowTitle(const char*) = 0;
		virtual void SetWindowIcon(const string&) = 0;

		void Run();
		void ScreenResolution(int& width, int& height);
		const eGraphicAPI& GetGraphicApi() const;

	protected:
		System();

		virtual bool InitializeWindows() = 0;
		virtual void ShutdownWindows() = 0;

	protected:
		static unique_ptr<System>	m_Instance;
		eGraphicAPI					m_Api;
	};
}
