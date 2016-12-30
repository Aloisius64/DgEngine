////////////////////////////////////////////////////////////////////////////////
// Filename: UI.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <AntTweakBar.h>
#include <memory>
#include <string>

#pragma comment (lib, "AntTweakBar.lib")    /* link AntTweakBar lib */

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Common.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


/////////////////////
// ANT TWEAK TYPES //
/////////////////////
extern TwType TW_TYPE_VEC2;
extern TwType TW_TYPE_VEC3;
extern TwType TW_TYPE_LIGHT;
extern TwType TW_TYPE_WRAP;
extern TwType TW_TYPE_FILTER;
extern TwType TW_TYPE_FOG;
extern TwType TW_TYPE_BLEND_FUNC;
extern TwType TW_TYPE_BLEND_EQUATION;
extern TwType TW_TYPE_RENDER_MODE;
extern TwType TW_TYPE_SHADOW;
extern TwType TW_TYPE_CAMERA_PROJECTION;

namespace dg {
	class SceneObject;
	class Material;
	class Texture;

	class UI {
	public:
		static const unique_ptr<UI>& Instance();
		static void Release();

		// Events.
		static void KeyBoadCallback(eKey, eKeyState);
		static void CursorPositionCallback(int, int);
		static void MouseCallback(eMouseButton, eKeyState);
		static int ConvertToAntTweakBarKey(eKey);
		
		static void RenderUI();
		static void DrawUI(const bool&);

	public:
		virtual ~UI();
		
		// Scene bar.
		void InitializeSceneBar();								// It starts from "Root" node.
		void InitializeSceneBar(const SceneObject*, int = 0);	// It starts from the given node.
		void UpdateSceneBar();
		void ReleaseSceneBar();

		// Active scene object bar.
		void SetActiveSceneObject(const string&);
		void InitializeActiveSceneObjectBar(SceneObject*);
		void ReleaseActiveSceneObjectBar();

		// Active material bar.
		void SetActiveMaterial(Material*);
		void InitializeActiveMaterialBar(Material*);
		void ReleaseActiveMaterialBar();

		// Active texture bar.
		void SetActiveTexture(Texture*);
		void InitializeActiveTextureBar(Texture*);
		void ReleaseActiveTextureBar();
		
		// Environment bar.
		void InitializeEnvironmentBar();
		void ReleaseEnvironmentBar();

		// Textures bar.
		void InitializeTexturesBar();
		void ReleaseTexturesBar();

		// Materials bar.
		void InitializeMaterialsBar();
		void ReleaseMaterialsBar();
		
	private:
		UI();

		bool Initialize();

	private:
		static unique_ptr<UI>	m_Instance;
		TwBar*					m_SceneBar;
		string					m_ActiveSceneObject;
		TwBar*					m_ActiveSceneObjectBar;
		Material*				m_ActiveMaterial;
		TwBar*					m_ActiveMaterialBar;
		Texture*				m_ActiveTexture;
		TwBar*					m_ActiveTextureBar;
		TwBar*					m_EnvironmentBar;
		TwBar*					m_TexturesBar;
		TwBar*					m_MaterialsBar;
		bool					m_DrawUI;
	};
}
