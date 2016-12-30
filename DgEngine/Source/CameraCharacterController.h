////////////////////////////////////////////////////////////////////////////////
// Filename: CameraCharacterController.h
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
#include "Physics.h"
#include "TimeClock.h"
#include "Transform.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


class CameraCharacterController : public Script {
public:
	CameraCharacterController(float walkSpeed = 0.64f, float jumpForce = 16.0f)
		: Script("CameraCharacterController") {
		//	Character controller
		m_JumpForce = jumpForce;
		m_WalkSpeed = walkSpeed;
		m_CurrentJumpForce = 0.0f;

		// Camera
		m_Camera = Camera::ActiveCamera();

		m_Speed = 32.0f;
		m_LastX = 400;
		m_LastY = 300;
		m_FirstMouse = true;
		m_MouseSensitivity = 0.25f;
		m_Yaw = 180.0f;
		m_Pitch = 0.0f;
	}

	virtual void OnActivate() {
		m_Physics = GetComponentType(GetSceneObject(), Physics);
		m_CameraTransform = GetComponentType(GetSceneObject(), Transform);
	}

	virtual void Update() {
		m_Camera->SetPosition(m_CameraTransform->GetPosition());
		vec3 cameraMovement(0.0f, 0.0f, 0.0f);

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
			cameraMovement += (m_Target * m_Speed * TimeClock::Instance()->GetDeltaTime());
		}
		if(Input::Instance()->IsKeyPress(eKey::KEY_DOWN)
			||Input::Instance()->IsKeyPress(eKey::KEY_S)
			||Input::Instance()->IsKeyHold(eKey::KEY_DOWN)
			||Input::Instance()->IsKeyHold(eKey::KEY_S)) {
			cameraMovement -= (m_Target * m_Speed * TimeClock::Instance()->GetDeltaTime());
		}
		if(Input::Instance()->IsKeyPress(eKey::KEY_RIGHT)
			||Input::Instance()->IsKeyPress(eKey::KEY_D)
			||Input::Instance()->IsKeyHold(eKey::KEY_RIGHT)
			||Input::Instance()->IsKeyHold(eKey::KEY_D)) {
			cameraMovement += Right;
		}
		if(Input::Instance()->IsKeyPress(eKey::KEY_LEFT)
			||Input::Instance()->IsKeyPress(eKey::KEY_A)
			||Input::Instance()->IsKeyHold(eKey::KEY_LEFT)
			||Input::Instance()->IsKeyHold(eKey::KEY_A)) {
			cameraMovement -= Right;
		}
		if(Input::Instance()->IsKeyPress(eKey::KEY_E)
			||Input::Instance()->IsKeyHold(eKey::KEY_E)) {
			cameraMovement += up;
		}
		if(Input::Instance()->IsKeyPress(eKey::KEY_Q)
			||Input::Instance()->IsKeyHold(eKey::KEY_Q)) {
			cameraMovement -= up;
		}

		float xpos = (float) Input::Instance()->GetMouseX();
		float ypos = (float) Input::Instance()->GetMouseY();

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

		if(Input::Instance()->IsKeyPress(eKey::KEY_SPACE)) {
			m_CurrentJumpForce = m_JumpForce;
		}

		if(m_Physics) {
			m_Physics->Move(cameraMovement*m_WalkSpeed);
			m_Physics->Jump(m_CurrentJumpForce);
		} else {
			m_CameraTransform->Translate(cameraMovement*m_WalkSpeed);
		}
		m_CurrentJumpForce = 0.0f;
	}

private:
	// Components.
	Camera*			m_Camera;
	Physics*		m_Physics;
	Transform*		m_CameraTransform;
	// Character controller.
	float 			m_WalkSpeed;
	float 			m_JumpForce;
	float 			m_CurrentJumpForce;
	// Camera.
	float			m_Speed;
	float			m_LastX;
	float			m_LastY;
	bool			m_FirstMouse;
	float			m_MouseSensitivity;
	float			m_Yaw;
	float			m_Pitch;
};
