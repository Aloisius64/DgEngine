////////////////////////////////////////////////////////////////////////////////
// Filename: Utils.cpp
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Utils.h"

bool dg::ReadFile(const char* pFileName, std::string& outFile) {
	std::ifstream f(pFileName);

	bool ret = false;

	if(f.is_open()) {
		std::string line;
		while(getline(f, line)) {
			outFile.append(line);
			outFile.append("\n");
		}

		f.close();

		ret = true;
	} else {
		std::string tmp = "Unable to open file";
		tmp += pFileName;
		DebugConsoleMessage(tmp.c_str());
	}

	return ret;
}

void dg::GetScreenResolution(int& width, int& height) {
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	width = desktop.right;
	height = desktop.bottom;
}

std::wstring dg::stringToLPCWSTR(const std::string& s) {
	int len;
	int slength = (int) s.length()+1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

void dg::InfoGLError(unsigned int errorCode) {
	char msg[512];
	memset(msg, 0, sizeof(msg));

	switch(errorCode) {
	case GL_NO_ERROR:
		SNPRINTF(msg, sizeof(msg), "CODE %d. GL_NO_ERROR.\n", errorCode);
		DebugConsoleMessage(msg);
		break;
		//No error has been recorded.The value of this symbolic constant is guaranteed to be 0.
	case GL_INVALID_ENUM:
		SNPRINTF(msg, sizeof(msg), "CODE %d. GL_INVALID_ENUM.\n", errorCode);
		DebugConsoleMessage(msg);
		break;
		//An unacceptable value is specified for an enumerated argument.The offending command is ignored and has no other side effect than to set the error flag.
	case GL_INVALID_VALUE:
		SNPRINTF(msg, sizeof(msg), "CODE %d. GL_INVALID_VALUE.\n", errorCode);
		DebugConsoleMessage(msg);
		break;
		//A numeric argument is out of range.The offending command is ignored and has no other side effect than to set the error flag.
	case GL_INVALID_OPERATION:
		SNPRINTF(msg, sizeof(msg), "CODE %d. GL_INVALID_OPERATION.\n", errorCode);
		DebugConsoleMessage(msg);
		break;
		//The specified operation is not allowed in the current state.The offending command is ignored and has no other side effect than to set the error flag.
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		SNPRINTF(msg, sizeof(msg), "CODE %d. GL_INVALID_FRAMEBUFFER_OPERATION.\n", errorCode);
		DebugConsoleMessage(msg);
		break;
		//The framebuffer object is not complete.The offending command is ignored and has no other side effect than to set the error flag.
	case GL_OUT_OF_MEMORY:
		SNPRINTF(msg, sizeof(msg), "CODE %d. GL_OUT_OF_MEMORY.\n", errorCode);
		DebugConsoleMessage(msg);
		break;
		//There is not enough memory left to execute the command.The state of the GL is undefined, except for the state of the error flags, after this error is recorded.
	case GL_STACK_UNDERFLOW:
		SNPRINTF(msg, sizeof(msg), "CODE %d. GL_STACK_UNDERFLOW.\n", errorCode);
		DebugConsoleMessage(msg);
		break;
		//An attempt has been made to perform an operation that would cause an internal stack to underflow.
	case GL_STACK_OVERFLOW:
		SNPRINTF(msg, sizeof(msg), "CODE %d. GL_STACK_OVERFLOW.\n", errorCode);
		DebugConsoleMessage(msg);
		break;
		//An attempt has been made to perform an operation that would cause an internal stack to overflow.
	default:
		SNPRINTF(msg, sizeof(msg), "Invalid error code: %d. \n", errorCode);
		DebugConsoleMessage(msg);
		break;
	}
}

void dg::InfoGLFramebufferError(unsigned int errorCode) {
	char msg[512];
	memset(msg, 0, sizeof(msg));

	switch(errorCode) {
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		SNPRINTF(msg, sizeof(msg), "CODE %d. GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT.\n", errorCode);
		DebugConsoleMessage(msg);
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
		SNPRINTF(msg, sizeof(msg), "CODE %d. GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS.\n", errorCode);
		DebugConsoleMessage(msg);
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		SNPRINTF(msg, sizeof(msg), "CODE %d. GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT.\n", errorCode);
		DebugConsoleMessage(msg);
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		SNPRINTF(msg, sizeof(msg), "CODE %d. GL_FRAMEBUFFER_UNSUPPORTED.\n", errorCode);
		DebugConsoleMessage(msg);
		break;
	default:
		SNPRINTF(msg, sizeof(msg), "Invalid error code: %d.\n", errorCode);
		DebugConsoleMessage(msg);
		break;
	}
}
