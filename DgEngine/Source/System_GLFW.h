////////////////////////////////////////////////////////////////////////////////
// Filename: System_GLFW.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <string>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "System.h"
#include "Common.h"


namespace dg {
	class System_GLFW : public System{
	public:
		static eKey ConvertGLFWKey(unsigned int);
		static eKeyState ConvertGLFWKeyState(unsigned int);
		static eMouseButton ConvertGLFWMouse(unsigned int);

	public:
		System_GLFW();
		virtual ~System_GLFW();

		virtual bool Initialize();
		virtual bool EndRun();
		virtual void SwapBuffers() const;
		virtual void ClearBuffers(glm::vec4) const;
		virtual void PollEvents() const;
		virtual void UpdateVieport(int, int, int, int) const;
		virtual void ShowCursor();
		virtual void HideCursor();
		virtual void SetWindowTitle(const char*);
		virtual void SetWindowIcon(const string&);

	private:
		virtual bool InitializeWindows();
		virtual void ShutdownWindows();

	private:
		std::string		m_ApplicationName;
	};

}
