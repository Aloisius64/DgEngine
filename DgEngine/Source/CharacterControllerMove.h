////////////////////////////////////////////////////////////////////////////////
// Filename: CharacterControllerMove.h
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


#define LIMIT_ROTATION_X 45.0f

class CharacterControllerMove : public Script {
public:
	CharacterControllerMove(float walkSpeed = 1.0f, float jumpForce = 16.0f)
		: Script("CharacterControllerMove"),
		movement(0.0f, 0.0f, 0.0f), jumpForce(jumpForce) {
		this->walkSpeed = walkSpeed;
		currentJumpForce = 0.0f;

		resetMovement();

		old_mouse_x = -1;
		old_mouse_y = -1;
		delta_mouse_x = -1;
		delta_mouse_y = -1;

		minDistanceToCamera = 20.0f;
		maxDistanceToCamera = 30.0f;
		mouseSensibility = 0.25f;
		totalRotationX = 0.0f;
	}

	virtual void Update() {
		//vec3 cameraPosition = Camera::MainCamera()->GetPosition();
		//Transform* transform = GetComponentType(GetSceneObject(), Transform);

		//resetMovement();

		//vec3 characterPosition = transform->GetPosition();
		//vec3 reverseTarget = Camera::MainCamera()->GetTarget();
		//reverseTarget *= -1;
		//Camera::MainCamera()->SetPosition(characterPosition+reverseTarget*minDistanceToCamera);

		///** Character update **/
		//if(Input::Instance()->IsKeyPress(eKey::KEY_UP)||Input::Instance()->IsKeyPress(eKey::KEY_W))
		//	backwardMove = true;
		//if(Input::Instance()->IsKeyPress(eKey::KEY_RIGHT)||Input::Instance()->IsKeyPress(eKey::KEY_D))
		//	rightMove = true;
		//if(Input::Instance()->IsKeyPress(eKey::KEY_DOWN)||Input::Instance()->IsKeyPress(eKey::KEY_S))
		//	forwardMove = true;
		//if(Input::Instance()->IsKeyPress(eKey::KEY_LEFT)||Input::Instance()->IsKeyPress(eKey::KEY_A))
		//	leftMove = true;
		//if(Input::Instance()->IsKeyPress(eKey::KEY_SPACE))
		//	currentJumpForce = jumpForce;

		//vec3 forward = cameraPosition-transform->GetPosition();
		//vec3 targetKeyDisplacement(0.0f);
		//forward.y = 0;
		//forward = normalize(forward);
		//vec3 up(0.0f, 1.0f, 0.0f);
		//vec3 right = cross(forward, up);
		//{
		//	if(forwardMove)		targetKeyDisplacement += forward;
		//	if(backwardMove)	targetKeyDisplacement -= forward;
		//	if(rightMove)		targetKeyDisplacement += right;
		//	if(leftMove)		targetKeyDisplacement -= right;

		//	//targetKeyDisplacement *= mKeyShiftDown ? mRunningSpeed : mWalkingSpeed;
		//	targetKeyDisplacement *= walkSpeed;
		//	//targetKeyDisplacement *= DgAppManager::getDeltaTime();

		//	//targetPadDisplacement += forward * mGamepadForwardInc * mGamepadWalkingSpeed;
		//	//targetPadDisplacement += right * mGamepadLateralInc * mGamepadWalkingSpeed;
		//	//targetPadDisplacement *= dtime;
		//}
		//movement = targetKeyDisplacement;
		//Physics* physics = GetComponentType(GetSceneObject(), Physics);
		//physics->Move(movement);
		//physics->Jump(currentJumpForce);

		///** Camera update **/
		//if(Input::Instance()->GetMouseX()>0) {
		//	if(old_mouse_x>=0) {
		//		delta_mouse_x = Input::Instance()->GetMouseX()-old_mouse_x;
		//		old_mouse_x = Input::Instance()->GetMouseX();
		//	} else {
		//		delta_mouse_x = 0;
		//		old_mouse_x = Input::Instance()->GetMouseX();
		//	}
		//}
		//if(Input::Instance()->GetMouseY()>0) {
		//	if(old_mouse_y>=0) {
		//		delta_mouse_y = Input::Instance()->GetMouseY()-old_mouse_y;
		//		old_mouse_y = Input::Instance()->GetMouseY();
		//	} else {
		//		delta_mouse_y = 0;
		//		old_mouse_y = Input::Instance()->GetMouseY();
		//	}
		//} else {
		//	delta_mouse_y = 0;
		//}
		//mat4 trasToOrigin = glm::translate(mat4(1.0f), -transform->GetPosition());
		//mat4 trasToCharacter = glm::translate(mat4(1.0f), transform->GetPosition());

		//float rotationXAxis = delta_mouse_y*mouseSensibility;
		//float rotationYAxis = delta_mouse_x*mouseSensibility;

		//totalRotationX += rotationXAxis;
		//if(totalRotationX>=LIMIT_ROTATION_X||totalRotationX<-LIMIT_ROTATION_X) {
		//	rotationXAxis = 0.0f;
		//}

		//mat4 rotationMatrix = glm::rotate(rotationXAxis, rotationYAxis, 0.0f);

		//vec4 cameraPos(Camera::MainCamera()->GetPosition()[0],
		//	Camera::MainCamera()->GetPosition()[1],
		//	Camera::MainCamera()->GetPosition()[2],
		//	1.0f);
		//vec4 newPosition4f = cameraPos*
		//	trasToCharacter *
		//	rotationMatrix *
		//	trasToOrigin;
		//vec3 newPosition3f(newPosition4f[0], newPosition4f[1], newPosition4f[2]);

		//// Correct min and max distance
		////vec3 diffPositions = newPosition3f-transform->GetPosition();
		////float distance = diffPositions.magnitude();
		////if(distance<minDistanceToCamera) {
		////	float diff = minDistanceToCamera-distance;
		////	newPosition3f[0] += diff;
		////	newPosition3f[2] += diff;
		////} else if(distance>maxDistanceToCamera) {
		////	//float diff = distance-minDistanceToCamera;
		////	//newPosition3f[0] -= diff;
		////	//newPosition3f[2] -= diff;
		////	newPosition3f -= diffPositions;
		////}

		//Camera::MainCamera()->SetPosition(newPosition3f);
		//vec3 newTarget = transform->GetPosition()-newPosition3f;
		//newTarget = normalize(newTarget);
		//Camera::MainCamera()->SetTarget(newTarget);

		///** Reset values **/
		//movement[0] = 0.0f;
		//movement[1] = 0.0f;
		//movement[2] = 0.0f;
		//currentJumpForce = 0.0f;
	}

private:
	void resetMovement() {
		this->forwardMove = false;
		this->backwardMove = false;
		this->rightMove = false;
		this->leftMove = false;
	}

private:
	vec3 movement;
	float walkSpeed;
	const float jumpForce;
	float currentJumpForce;

	bool forwardMove;
	bool backwardMove;
	bool rightMove;
	bool leftMove;

	float minDistanceToCamera;
	float maxDistanceToCamera;
	float mouseSensibility;
	float totalRotationX;
	int old_mouse_x;
	int old_mouse_y;
	int delta_mouse_x;
	int delta_mouse_y;
};
