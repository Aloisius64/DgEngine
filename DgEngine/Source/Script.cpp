////////////////////////////////////////////////////////////////////////////////
// Filename: Script.cpp
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Script.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


#pragma region AntTweakBar
static void TW_CALL EnabledCallback(const void *value, void *clientData) {
	static_cast<Script *>(clientData)->Enable(*static_cast<const bool *>(value));
}
static void TW_CALL IsEnabledCallback(void *value, void *clientData) {
	*static_cast<bool *>(value) = static_cast<Script*>(clientData)->IsEnabled();
}
#pragma endregion

Script::Script(const string& scriptName) {
	m_SceneObject = nullptr;
	m_Name = scriptName;
	Enable();
	m_Activated = false;
}

Script::~Script() {
	m_SceneObject = nullptr;
}

void Script::Enable(bool value) {
	m_Enabled = value;
}

void Script::Disable() {
	m_Enabled = false;
}

bool Script::IsEnabled() const {
	return m_Enabled;
}

const string& Script::GetName() const {
	return m_Name;
}

void Script::SetSceneObject(SceneObject* sceneObject) {
	if (!m_SceneObject) {
		m_SceneObject = sceneObject;
	}
}

SceneObject* Script::GetSceneObject() {
	return m_SceneObject;
}

void Script::OnActivate() {

}

void Script::Update() {

}

void Script::PostUpdate() {

}

void Script::Release() {

}

void Script::DrawUI(TwBar* bar) {
	TwAddVarCB(bar, m_Name.c_str(), TW_TYPE_BOOL8, EnabledCallback, IsEnabledCallback, this, "");
}

void Script::OnTriggerStart(SceneObject* othersceneObject, const PxTriggerPair pairs) {

}

void Script::OnTriggerEnd(SceneObject* othersceneObject, const PxTriggerPair pairs) {

}

void Script::OnColliderStart(SceneObject* othersceneObject, const PxContactPair pairs) {

}

void Script::OnColliderPersist(SceneObject* othersceneObject, const PxContactPair pairs) {

}

void Script::OnColliderEnd(SceneObject* othersceneObject, const PxContactPair pairs) {

}

void Script::OnShapeHit(const PxControllerShapeHit& hit) {

}

void Script::OnControllerHit(const PxControllersHit& hit) {

}

void Script::OnObstacleHit(const PxControllerObstacleHit& hit) {

}
