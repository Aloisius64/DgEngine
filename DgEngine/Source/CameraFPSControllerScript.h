////////////////////////////////////////////////////////////////////////////////
// Filename: CameraFPSControllerScript.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Script.h"
#include "Camera.h"
#include "Input.h"
#include "Mathematics.h"
#include "TimeClock.h"
#include "Transform.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


class CameraFPSControllerScript : public Script {
public:
	CameraFPSControllerScript()
		: Script("CameraFPSControllerScript") {
		m_Camera = Camera::ActiveCamera();

		m_Speed = 32.0f;
		m_LastX = 400;
		m_LastY = 300;
		m_FirstMouse = true;
		m_MouseSensitivity = 0.25f;
		m_Yaw = 180.0f;
		m_Pitch = 0.0f;

		m_LastMouseState = eKeyState::KEY_STATE_UNDEFINED;
	}

	virtual void Update() {
		eKeyState mouseState = Input::MouseState(eMouseButton::MOUSE_BUTTON_RIGHT);

		float xpos = (float) Input::Instance()->GetMouseX();
		float ypos = (float) Input::Instance()->GetMouseY();

		if(mouseState==eKeyState::KEY_STATE_PRESS && m_LastMouseState==eKeyState::KEY_STATE_RELEASE) {
			m_LastX = xpos;
			m_LastY = ypos;
		}
		if(mouseState!=m_LastMouseState) {
			m_LastMouseState = mouseState;
		}

		if(!(mouseState==eKeyState::KEY_STATE_PRESS)&&
			!(mouseState==eKeyState::KEY_STATE_HOLD)&&
			!Input::IsKeyPress(eKey::KEY_SPACE)&&
			!Input::IsKeyHold(eKey::KEY_SPACE)) {
			return;
		}
		
		vec3 m_Target = m_Camera->GetTarget();
		vec3 position = m_Camera->GetPosition();
		vec3 up = m_Camera->GetUp();
		vec3 Right = normalize(glm::cross(m_Target, up));
		float movement_step = m_Speed * TimeClock::Instance()->GetDeltaTime();
		up *= movement_step;
		Right *= movement_step;

		if(Input::Instance()->IsKeyPress(eKey::KEY_UP)
			||Input::Instance()->IsKeyPress(eKey::KEY_W)
			||Input::Instance()->IsKeyHold(eKey::KEY_UP)
			||Input::Instance()->IsKeyHold(eKey::KEY_W)) {
			position += (m_Target * m_Speed * TimeClock::Instance()->GetDeltaTime());
			m_Camera->SetPosition(position);
		}
		if(Input::Instance()->IsKeyPress(eKey::KEY_DOWN)
			||Input::Instance()->IsKeyPress(eKey::KEY_S)
			||Input::Instance()->IsKeyHold(eKey::KEY_DOWN)
			||Input::Instance()->IsKeyHold(eKey::KEY_S)) {
			position -= (m_Target * m_Speed * TimeClock::Instance()->GetDeltaTime());
			m_Camera->SetPosition(position);
		}
		if(Input::Instance()->IsKeyPress(eKey::KEY_RIGHT)
			||Input::Instance()->IsKeyPress(eKey::KEY_D)
			||Input::Instance()->IsKeyHold(eKey::KEY_RIGHT)
			||Input::Instance()->IsKeyHold(eKey::KEY_D)) {
			position += Right;
			m_Camera->SetPosition(position);
		}
		if(Input::Instance()->IsKeyPress(eKey::KEY_LEFT)
			||Input::Instance()->IsKeyPress(eKey::KEY_A)
			||Input::Instance()->IsKeyHold(eKey::KEY_LEFT)
			||Input::Instance()->IsKeyHold(eKey::KEY_A)) {
			position -= Right;
			m_Camera->SetPosition(position);
		}
		if(Input::Instance()->IsKeyPress(eKey::KEY_E)
			||Input::Instance()->IsKeyHold(eKey::KEY_E)) {
			position += up;
			m_Camera->SetPosition(position);
		}
		if(Input::Instance()->IsKeyPress(eKey::KEY_Q)
			||Input::Instance()->IsKeyHold(eKey::KEY_Q)) {
			position -= up;
			m_Camera->SetPosition(position);
		}

		if(m_FirstMouse) {
			m_LastX = xpos;
			m_LastY = ypos;
			m_FirstMouse = false;
		}

		float xoffset = m_LastX-xpos;
		float yoffset = m_LastY-ypos;

		m_LastX = xpos;
		m_LastY = ypos;

		xoffset *= m_MouseSensitivity;
		yoffset *= m_MouseSensitivity;

		m_Yaw += xoffset;
		m_Pitch += yoffset;

		// Make sure that when m_Pitch is out of bounds, screen doesn't get flipped
		if(m_Pitch>89.0f)
			m_Pitch = 89.0f;
		if(m_Pitch<-89.0f)
			m_Pitch = -89.0f;
		
		m_Camera->SetRotation(vec3(-m_Pitch, m_Yaw, 0.0f));
	}

private:
	Camera*			m_Camera;
	eKeyState		m_LastMouseState;
	float			m_Speed;
	float			m_LastX;
	float			m_LastY;
	bool			m_FirstMouse;
	float			m_MouseSensitivity;
	float			m_Yaw;
	float			m_Pitch;
};
