////////////////////////////////////////////////////////////////////////////////
// Filename: Component.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Component.h"
#include "SceneObject.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


Component::Component(eComponentType componentType, SceneObject* sceneObject) {
	m_SceneObject = sceneObject;
	m_ComponentType = componentType;
	Enable();
	// Set the relative component to the given scene object.
	if(sceneObject) {
		sceneObject->SetComponent(this);
	}
}

Component::~Component() {
	m_SceneObject = nullptr;
}

void Component::Update() {

}

void Component::PostUpdate() {

}

void Component::Render(const shared_ptr<Shader>&, bool, const GLuint&) {

}

void Component::DrawUI(TwBar*) {

}

void Component::Enable(bool value) {
	m_Enabled = value;
}

void Component::Disable() {
	m_Enabled = false;
}

bool Component::IsEnabled() const{
	return m_Enabled;
}

SceneObject* Component::GetSceneObject() const {
	return m_SceneObject;
}

const eComponentType& Component::ComponentType() const {
	return m_ComponentType;
}
