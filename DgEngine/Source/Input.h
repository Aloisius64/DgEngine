////////////////////////////////////////////////////////////////////////////////
// Filename: Input.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <map>
#include <memory>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Common.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	typedef map<eKey, eKeyState, less<eKey>> KeyboardMap;
	typedef map<eMouseButton, eKeyState, less<eMouseButton>> MouseMap;

	class Input {
	public:
		static const unique_ptr<Input>& Instance();
		static void Release();
		static eKeyState KeyState(eKey);
		static void KeyState(eKey, eKeyState);
		static bool IsKeyPress(eKey);
		static bool IsKeyRelease(eKey);
		static bool IsKeyHold(eKey);
		static void MousePosition(int, int);
		static int GetMouseX();
		static int GetMouseY();
		static void MouseWheelOffset(double, double);
		static double GetMouseWheelXOffset();
		static double GetMouseWheelYOffset();
		static eKeyState MouseState(eMouseButton);
		static void MouseState(eMouseButton, eKeyState);

	public:
		virtual ~Input();

	private:
		Input();

	private:
		static unique_ptr<Input>	m_Instance;
		// Keyboard
		KeyboardMap					m_KeyStateMap;
		// Mouse
		MouseMap					m_MouseStateMap;
		int							m_MouseX;
		int							m_MouseY;
		double						m_MouseWheelXOffset;
		double						m_MouseWheelYOffset;
	};
}
