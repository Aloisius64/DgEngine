////////////////////////////////////////////////////////////////////////////////
// Filename: Component.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <cstdlib>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Common.h"
#include "Mathematics.h"
#include "UI.h"


////////////////
// NAMESPACES //
////////////////


namespace dg {
	class SceneObject;
	class Shader;

	class Component {
		friend class SceneObject;	
	public:
		static const eComponentType& ComponentType(Component*);

	public:
		Component(eComponentType, SceneObject* = nullptr);
		virtual ~Component();

		virtual void Update();
		virtual void PostUpdate();
		virtual void Render(const shared_ptr<Shader>& = nullptr, bool = false, const GLuint& = NULL_QUERY);
		virtual void DrawUI(TwBar*);

		const eComponentType& ComponentType() const;
		SceneObject* GetSceneObject() const;
		void Enable(bool = true);
		void Disable();
		bool IsEnabled() const;

#pragma region AntTweakBar
		static void TW_CALL EnabledCallback(const void *value, void *clientData) {
			static_cast<Component *>(clientData)->Enable(*static_cast<const bool *>(value));
		}
		static void TW_CALL IsEnabledCallback(void *value, void *clientData) {
			*static_cast<bool *>(value) = static_cast<Component*>(clientData)->IsEnabled();
		}
#pragma endregion

	protected:
		SceneObject*	m_SceneObject;

	private:
		eComponentType	m_ComponentType;
		bool			m_Enabled;
	};
}
