////////////////////////////////////////////////////////////////////////////////
// Filename: Input.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Input.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


unique_ptr<Input> Input::m_Instance = nullptr;

const unique_ptr<Input>& Input::Instance() {
	if(!m_Instance) {
		m_Instance = unique_ptr<Input>(new Input());
	}
	return m_Instance;
}

Input::Input() {

}

Input::~Input() {}

void Input::Release() {
	m_Instance = nullptr;
}

void Input::KeyState(eKey key, eKeyState state) {
	Instance()->m_KeyStateMap[key] = state;
}

eKeyState Input::KeyState(eKey key){
	if(Instance()->m_KeyStateMap.find(key)==Instance()->m_KeyStateMap.end()) {
		Instance()->m_KeyStateMap[key] = eKeyState::KEY_STATE_UNDEFINED;
	}
	return Instance()->m_KeyStateMap[key];
}

bool Input::IsKeyPress(eKey key){
	return KeyState(key)==eKeyState::KEY_STATE_PRESS;
}

bool Input::IsKeyRelease(eKey key){
	return KeyState(key)==eKeyState::KEY_STATE_RELEASE;
}

bool Input::IsKeyHold(eKey key){
	return KeyState(key)==eKeyState::KEY_STATE_HOLD;
}

void Input::MousePosition(int x, int y) {
	Instance()->m_MouseX = x;
	Instance()->m_MouseY = y;
}

int Input::GetMouseX() {
	return Instance()->m_MouseX;
}

int Input::GetMouseY() {
	return Instance()->m_MouseY;
}

void Input::MouseWheelOffset(double xOffset, double yOffset) {
	Instance()->m_MouseWheelXOffset = xOffset;
	Instance()->m_MouseWheelYOffset = yOffset;
}

double Input::GetMouseWheelXOffset() {
	return Instance()->m_MouseWheelXOffset;
}

double Input::GetMouseWheelYOffset() {
	return Instance()->m_MouseWheelYOffset;
}

eKeyState Input::MouseState(eMouseButton mouseButton) {
	if(Instance()->m_MouseStateMap.find(mouseButton)==Instance()->m_MouseStateMap.end()) {
		Instance()->m_MouseStateMap[mouseButton] = eKeyState::KEY_STATE_UNDEFINED;
	}
	return Instance()->m_MouseStateMap[mouseButton];
}

void Input::MouseState(eMouseButton mouseButton, eKeyState state) {
	Instance()->m_MouseStateMap[mouseButton] = state;
}
