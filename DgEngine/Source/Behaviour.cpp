////////////////////////////////////////////////////////////////////////////////
// Filename: Behaviour.cpp
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <vector>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Behaviour.h"
#include "Script.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


#pragma region AntTweakBar
static void TW_CALL GetNumScriptsCallback(void *value, void *clientData) {
	*static_cast<unsigned int *>(value) = static_cast<Behaviour*>(clientData)->GetScripts().size();
}
#pragma endregion

Behaviour::Behaviour()
	: Component(eComponentType::BEHAVIOUR) {
	//GetBehaviorFlagsShapeActor = nullptr;
	//GetBehaviorFlagsController = nullptr;
	//GetBehaviorFlagsObstacle = nullptr;
}
Behaviour::Behaviour(SceneObject* gameObject)
	: Component(eComponentType::BEHAVIOUR, gameObject) {
	//GetBehaviorFlagsShapeActor = nullptr;
	//GetBehaviorFlagsController = nullptr;
	//GetBehaviorFlagsObstacle = nullptr;
}
Behaviour::~Behaviour() {
	for each (auto &script in m_Scripts)
		script->Release();

	for each (auto script in m_Scripts)
		FREE_PTR(script);
	m_Scripts.clear();
}

void Behaviour::AddScript(Script* script) {
	m_Scripts.push_back(script);
	script->SetSceneObject(GetSceneObject());
}

void Behaviour::RemoveScript(Script* scriptToRemove) {
	for each (auto script in m_Scripts)
		if(scriptToRemove==script) {
			script->Release();
			FREE_PTR(script);
			break;
		}
}

const vector<Script*>& Behaviour::GetScripts() const {
	return m_Scripts;
}

void Behaviour::Update() {
	for each (auto script in m_Scripts)
		if(script->IsEnabled()) {
			if(!script->m_Activated && script->IsEnabled()) {
				script->OnActivate();
				script->m_Activated = true;
			}
			script->Update();
		}
}

void Behaviour::PostUpdate() {
	for each (auto script in m_Scripts) {
		if(script->IsEnabled()) {
			script->PostUpdate();
		}
	}
}

void Behaviour::DrawUI(TwBar* bar) {
	char parameter[128];
	memset(parameter, 0, sizeof(parameter));

	TwAddButton(bar, "Behaviour", NULL, NULL, "");
	TwAddVarCB(bar, "Behaviour Enabled", TW_TYPE_BOOL8, EnabledCallback, IsEnabledCallback, this, "");
	TwAddVarCB(bar, "NumScript:", TW_TYPE_UINT32, NULL, GetNumScriptsCallback, this, "");
	for each (auto &script in GetScripts()) {
		script->DrawUI(bar);
	}
	TwAddSeparator(bar, "", NULL);
	TwAddSeparator(bar, "", NULL);
}

void Behaviour::OnTriggerStart(SceneObject* otherGameObject, const PxTriggerPair& pairs) {
	for each (auto script in m_Scripts)
		if(script->IsEnabled())
			script->OnTriggerStart(otherGameObject, pairs);
}

void Behaviour::OnTriggerEnd(SceneObject* otherGameObject, const PxTriggerPair& pairs) {
	for each (auto script in m_Scripts)
		if(script->IsEnabled())
			script->OnTriggerEnd(otherGameObject, pairs);
}

void Behaviour::OnColliderStart(SceneObject* otherGameObject, const PxContactPair& pairs) {
	for each (auto script in m_Scripts)
		if(script->IsEnabled())
			script->OnColliderStart(otherGameObject, pairs);
}

void Behaviour::OnColliderPersist(SceneObject* otherGameObject, const PxContactPair& pairs) {
	for each (auto script in m_Scripts)
		if(script->IsEnabled())
			script->OnColliderPersist(otherGameObject, pairs);
}

void Behaviour::OnColliderEnd(SceneObject* otherGameObject, const PxContactPair& pairs) {
	for each (auto script in m_Scripts)
		if(script->IsEnabled())
			script->OnColliderEnd(otherGameObject, pairs);
}

void Behaviour::OnShapeHit(const PxControllerShapeHit& hit) {
	for each (auto script in m_Scripts)
		if(script->IsEnabled())
			script->OnShapeHit(hit);
}

void Behaviour::OnControllerHit(const PxControllersHit& hit) {
	for each (auto script in m_Scripts)
		if(script->IsEnabled())
			script->OnControllerHit(hit);
}

void Behaviour::OnObstacleHit(const PxControllerObstacleHit& hit) {
	for each (auto script in m_Scripts)
		if(script->IsEnabled())
			script->OnObstacleHit(hit);
}

PxControllerBehaviorFlags Behaviour::GetBehaviorFlags(const PxShape& shape, const PxActor& actor) {
	if(this->GetBehaviorFlagsShapeActor) {
		return this->GetBehaviorFlagsShapeActor(shape, actor);
	}
	return PxControllerBehaviorFlags(0);
}

PxControllerBehaviorFlags Behaviour::GetBehaviorFlags(const PxController& controller) {
	if(this->GetBehaviorFlagsController) {
		this->GetBehaviorFlagsController(controller);
	}
	return PxControllerBehaviorFlags(0);
}

PxControllerBehaviorFlags Behaviour::GetBehaviorFlags(const PxObstacle& obstacle) {
	if(this->GetBehaviorFlagsObstacle) {
		this->GetBehaviorFlagsObstacle(obstacle);
	}
	return PxControllerBehaviorFlags(0);
}
