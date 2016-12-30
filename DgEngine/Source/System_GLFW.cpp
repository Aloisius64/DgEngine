////////////////////////////////////////////////////////////////////////////////
// Filename: System_GLFW.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////
#pragma region GLFW
#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define OVR_OS_WIN32
#elif defined(__APPLE__)
#define GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_NSGL
#define OVR_OS_MAC
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#define OVR_OS_LINUX
#endif

#ifdef WIN32
#include <Windows.h>
#endif
#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "gdi32.lib")    /* link Windows GDI lib        */
#pragma comment (lib, "winmm.lib")    /* link Windows MultiMedia lib */
#pragma comment (lib, "user32.lib")   /* link Windows user lib       */
#pragma endregion

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "System_GLFW.h"
#include "Configuration.h"
#include "ImageLoader.h"
#include "Input.h"
#include "Utils.h"
#include "UI.h"

using namespace dg;

static GLFWwindow* mainWindow = nullptr;

#pragma region CallbacksDefinition
static void ErrorCallback(int error, const char* description) {
	char msg[1000];
	_snprintf_s(msg, sizeof(msg), "GLFW error %d - %s", error, description);
	dg::DebugConsoleMessage(msg);
	exit(0);
}

static void KeyboardCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods) {
	eKey e_key = System_GLFW::ConvertGLFWKey(key);
	eKeyState e_keyState = System_GLFW::ConvertGLFWKeyState(action);
	Input::KeyState(e_key, e_keyState);
	UI::KeyBoadCallback(e_key, e_keyState);
}

static void CursorPositionCallback(GLFWwindow* pWindow, double x, double y) {
	Input::MousePosition((int) x, (int) y);
	UI::CursorPositionCallback((int) x, (int) y);
}

static void MouseCallback(GLFWwindow* pWindow, int button, int action, int mode) {
	eMouseButton e_mouseButton = System_GLFW::ConvertGLFWMouse(button);
	eKeyState e_keyState = System_GLFW::ConvertGLFWKeyState(action);
	Input::MouseState(e_mouseButton, e_keyState);
	double x, y;
	glfwGetCursorPos(pWindow, &x, &y);
	Input::MousePosition((int) x, (int) y);
	UI::MouseCallback(e_mouseButton, e_keyState);
}

static void MouseWheelScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	Input::MouseWheelOffset(xoffset, yoffset);
}

static void CursorCallback(GLFWwindow* window, int entered) {

}

static void WindowSizeCallback(GLFWwindow* window, int width, int height) {

}

static void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {

}

static void WindowPositionCallback(GLFWwindow* window, int xpos, int ypos) {

}

static void WindowIconifyCallback(GLFWwindow* window, int iconified) {

}

static void WindowCloseCallback(GLFWwindow* window) {

}

static void WindowFocusCallback(GLFWwindow* window, int focused) {

}
#pragma endregion

System_GLFW::System_GLFW() : System() {

}

System_GLFW::~System_GLFW() {

}

bool System_GLFW::Initialize() {
	// Initialize the windows api.
	if(!InitializeWindows()) {
		return false;
	}

#if !defined(_DEBUG) && defined(WIN32)
	FreeConsole();
#endif

	// Set GLFW callbacks.
	glfwSetErrorCallback(ErrorCallback);
	glfwSetKeyCallback(mainWindow, KeyboardCallback);
	glfwSetCursorPosCallback(mainWindow, CursorPositionCallback);
	glfwSetMouseButtonCallback(mainWindow, MouseCallback);
	glfwSetScrollCallback(mainWindow, MouseWheelScrollCallback);
	glfwSetCursorEnterCallback(mainWindow, CursorCallback);
	glfwSetWindowCloseCallback(mainWindow, WindowCloseCallback);
	glfwSetWindowSizeCallback(mainWindow, WindowSizeCallback);
	glfwSetFramebufferSizeCallback(mainWindow, FramebufferSizeCallback);
	glfwSetWindowPosCallback(mainWindow, WindowPositionCallback);
	glfwSetWindowIconifyCallback(mainWindow, WindowIconifyCallback);
	glfwSetWindowFocusCallback(mainWindow, WindowFocusCallback);

	// Initialize OpenGL
	glewExperimental = GL_TRUE;
	if(glewInit()==GLEW_OK) {
		if(Configuration::Instance()->IsDepthEnabled()) {
			glEnable(GL_DEPTH_TEST);
		}

		if(Configuration::Instance()->IsStencilEnabled()) {
			glEnable(GL_STENCIL);
		}

		if(Configuration::Instance()->IsCullingEnabled()) {
			glEnable(GL_CULL_FACE);
			glCullFace(Configuration::Instance()->GetCullingMode());
		}

		if(Configuration::Instance()->GetSamples()>0) {
			glEnable(GL_MULTISAMPLE);
		}

		if(Configuration::Instance()->IsBlendingEnabled()) {
			glEnable(GL_BLEND);
		}

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glFrontFace(Configuration::Instance()->GetPolygonMode());

		return true;
	}

	return false;
}

bool System_GLFW::EndRun() {
	return glfwWindowShouldClose(mainWindow)!=0;
}

void System_GLFW::SwapBuffers() const {
	glfwSwapBuffers(mainWindow);
}

void System_GLFW::ClearBuffers(glm::vec4 clearColor) const {
	glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
}

void System_GLFW::PollEvents() const {
	glfwPollEvents();
}

void System_GLFW::UpdateVieport(int x, int y, int width, int height) const {
	glViewport(x, y, width, height);
}

bool System_GLFW::InitializeWindows() {

	// Give the application a name.
	m_ApplicationName = Configuration::Instance()->ApplicationName();

	if(glfwInit()!=1) {
		dg::DebugConsoleMessage("Error initializing GLFW");
		return false;
	}

	GLFWmonitor* pMonitor = Configuration::Instance()->IsFullScreen() ? glfwGetPrimaryMonitor() : NULL;

	int screenWidth = Configuration::Instance()->GetScreenWidth();
	int screenHeight = Configuration::Instance()->GetScreenHeigth();

	// If fullscreen get the correct width and height
	if(Configuration::Instance()->IsFullScreen()) {
		dg::GetScreenResolution(screenWidth, screenHeight);
	}

	glfwWindowHint(GLFW_RESIZABLE, Configuration::Instance()->IsResizable());
	glfwWindowHint(GLFW_DECORATED, Configuration::Instance()->IsDecorated());
	glfwWindowHint(GLFW_FLOATING, Configuration::Instance()->IsFloating());
	glfwWindowHint(GLFW_SAMPLES, Configuration::Instance()->GetSamples());
	glfwWindowHint(GLFW_REFRESH_RATE, Configuration::Instance()->GetRefreshRate());
	glfwWindowHint(GLFW_FOCUSED, true);

	mainWindow = glfwCreateWindow(screenWidth, screenHeight, m_ApplicationName.c_str(), pMonitor, NULL);
	if(!mainWindow) {
		dg::DebugConsoleMessage("Unable to create window!");
		return false;
	}

	//	Set window position
	if(!Configuration::Instance()->IsFullScreen()) {
		if(Configuration::Instance()->IsScreenCentered()) {
			int xResolution = 0;
			int yResolution = 0;
			dg::GetScreenResolution(xResolution, yResolution);
			Configuration::Instance()->SetXScreenPosition((xResolution-screenWidth)/2);
			Configuration::Instance()->SetYScreenPosition((yResolution-screenHeight)/2);
		}
		glfwSetWindowPos(mainWindow,
			Configuration::Instance()->GetXScreenPosition(),
			Configuration::Instance()->GetYScreenPosition());
	}

	glfwMakeContextCurrent(mainWindow);
	glfwSwapInterval(1);

	return true;
}

void System_GLFW::ShutdownWindows() {
	// Release the pointer to this class.
	System::m_Instance = nullptr;

	// Release UI.
	UI::Release();

	glfwDestroyWindow(mainWindow);
	glfwTerminate();

	return;
}

eKey System_GLFW::ConvertGLFWKey(unsigned int key) {
	if(key>=GLFW_KEY_SPACE && key<=GLFW_KEY_RIGHT_BRACKET) {
		return (eKey) key;
	}

	switch(key) {
	case GLFW_KEY_ESCAPE:
		return eKey::KEY_ESCAPE;
	case GLFW_KEY_ENTER:
		return eKey::KEY_ENTER;
	case GLFW_KEY_TAB:
		return eKey::KEY_TAB;
	case GLFW_KEY_BACKSPACE:
		return eKey::KEY_BACKSPACE;
	case GLFW_KEY_INSERT:
		return eKey::KEY_INSERT;
	case GLFW_KEY_DELETE:
		return eKey::KEY_DELETE;
	case GLFW_KEY_RIGHT:
		return eKey::KEY_RIGHT;
	case GLFW_KEY_LEFT:
		return eKey::KEY_LEFT;
	case GLFW_KEY_DOWN:
		return eKey::KEY_DOWN;
	case GLFW_KEY_UP:
		return eKey::KEY_UP;
	case GLFW_KEY_PAGE_UP:
		return eKey::KEY_PAGE_UP;
	case GLFW_KEY_PAGE_DOWN:
		return eKey::KEY_PAGE_DOWN;
	case GLFW_KEY_HOME:
		return eKey::KEY_HOME;
	case GLFW_KEY_END:
		return eKey::KEY_END;
	case GLFW_KEY_F1:
		return eKey::KEY_F1;
	case GLFW_KEY_F2:
		return eKey::KEY_F2;
	case GLFW_KEY_F3:
		return eKey::KEY_F3;
	case GLFW_KEY_F4:
		return eKey::KEY_F4;
	case GLFW_KEY_F5:
		return eKey::KEY_F5;
	case GLFW_KEY_F6:
		return eKey::KEY_F6;
	case GLFW_KEY_F7:
		return eKey::KEY_F7;
	case GLFW_KEY_F8:
		return eKey::KEY_F8;
	case GLFW_KEY_F9:
		return eKey::KEY_F9;
	case GLFW_KEY_F10:
		return eKey::KEY_F10;
	case GLFW_KEY_F11:
		return eKey::KEY_F11;
	case GLFW_KEY_F12:
		return eKey::KEY_F12;
	default:
		//DG_ERROR("Unimplemented DG key");
		break;
	}

	return eKey::KEY_UNDEFINED;
}

eKeyState System_GLFW::ConvertGLFWKeyState(unsigned int state) {
	if(state==GLFW_RELEASE) {
		return eKeyState::KEY_STATE_RELEASE;
	} else if(state==GLFW_PRESS) {
		return eKeyState::KEY_STATE_PRESS;
	} else if(state==GLFW_REPEAT) {
		return eKeyState::KEY_STATE_HOLD;
	}
	return eKeyState::KEY_STATE_UNDEFINED;
}

eMouseButton System_GLFW::ConvertGLFWMouse(unsigned int button) {
	switch(button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		return eMouseButton::MOUSE_BUTTON_LEFT;
	case GLFW_MOUSE_BUTTON_RIGHT:
		return eMouseButton::MOUSE_BUTTON_RIGHT;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		return eMouseButton::MOUSE_BUTTON_MIDDLE;
	default:
		dg::DebugConsoleMessage("Unimplemented mouse button");
	}

	return eMouseButton::MOUSE_UNDEFINED;
}

void System_GLFW::ShowCursor() {
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void System_GLFW::HideCursor() {
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void System_GLFW::SetWindowTitle(const char* title) {
	glfwSetWindowTitle(mainWindow, title);
}

void System_GLFW::SetWindowIcon(const string& iconPath) {
	//if(!ImageLoader::LoadImg(iconPath))
	//	return;
	//GLFWimage image;
	//image.width = ImageLoader::GetImageWidth();
	//image.height = ImageLoader::GetImageHeight();
	//image.pixels = ImageLoader::GetImageData();
	//ImageLoader::FreeCPUMemory();
	//glfwSetWindowIcon(mainWindow, 1, &image);
}
