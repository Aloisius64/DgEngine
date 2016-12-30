////////////////////////////////////////////////////////////////////////////////
// Filename: UI.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "UI.h"
#include "Animator.h"
#include "ApplicationManager.h"
#include "ApplicationScene.h"
#include "Behaviour.h"
#include "Camera.h"
#include "Configuration.h"
#include "DataManager.h"
#include "Environment.h"
#include "Light.h"
#include "MeshRenderer.h"
#include "SceneObject.h"
#include "Script.h"
#include "Texture.h"
#include "Transform.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;

#pragma region TW_TYPES
TwType TW_TYPE_VEC2;
TwType TW_TYPE_VEC3;
TwType TW_TYPE_LIGHT;
TwType TW_TYPE_WRAP;
TwType TW_TYPE_FILTER;
TwType TW_TYPE_FOG;
TwType TW_TYPE_BLEND_FUNC;
TwType TW_TYPE_BLEND_EQUATION;
TwType TW_TYPE_RENDER_MODE;
TwType TW_TYPE_SHADOW;
TwType TW_TYPE_CAMERA_PROJECTION;
#pragma endregion

#pragma region Callbacks
void TW_CALL SceneObjectSelectedCallback(void *p) {
	const SceneObject* sceneObject = static_cast<const SceneObject *>(p);
	DataManager::SetActiveSceneObject(sceneObject);
	//dg::DebugConsoleMessage("Selected: ");
	//dg::DebugConsoleMessage(sceneObject->Id().c_str());
	//dg::DebugConsoleMessage("\n");
	UI::Instance()->SetActiveSceneObject(sceneObject->Id());
}

void TW_CALL SetActiveScript(void *p) {
	Script* script = static_cast<Script *>(p);
	if(script->IsEnabled())
		script->Enable();
	else
		script->Disable();
}
#pragma endregion

unique_ptr<UI> UI::m_Instance = nullptr;

void UI::KeyBoadCallback(eKey key, eKeyState keyState) {
	if(keyState==eKeyState::KEY_STATE_PRESS) {
		int atbKey = UI::ConvertToAntTweakBarKey(key);
		if(atbKey==TW_KEY_LAST) {
			return;
		}
		TwKeyPressed(atbKey, TW_KMOD_NONE);
	}
}

void UI::CursorPositionCallback(int x, int y) {
	// AntTweakBar.
	TwMouseMotion(x, y);
}

void UI::MouseCallback(eMouseButton mouse, eKeyState keyState) {
	// AntTweakBar.
	TwMouseButtonID atb_mouse_button;
	switch(mouse) {
	case eMouseButton::MOUSE_BUTTON_LEFT: {
		atb_mouse_button = TW_MOUSE_LEFT;
	}break;
	case eMouseButton::MOUSE_BUTTON_MIDDLE:{
		atb_mouse_button = TW_MOUSE_MIDDLE;
	} break;
	case eMouseButton::MOUSE_BUTTON_RIGHT: {
		atb_mouse_button = TW_MOUSE_RIGHT;
	}break;
	default: return;
	}

	TwMouseAction atb_mouse_action;
	switch(keyState) {
	case eKeyState::KEY_STATE_PRESS: {
		atb_mouse_action = TW_MOUSE_PRESSED;
	}break;
	case eKeyState::KEY_STATE_RELEASE: {
		atb_mouse_action = TW_MOUSE_RELEASED;
	}break;
	default: return;
	}

	TwMouseButton(atb_mouse_action, atb_mouse_button);
}

int UI::ConvertToAntTweakBarKey(eKey key) {
	if(key>=eKey::KEY_SPACE && key<=eKey::KEY_RIGHT_BRACKET) {
		return key;
	}

	switch(key) {
	case eKey::KEY_BACKSPACE:
		return TW_KEY_BACKSPACE;
	case eKey::KEY_TAB:
		return TW_KEY_TAB;
		//return TW_KEY_CLEAR;
	case eKey::KEY_ENTER:
		return TW_KEY_RETURN;
		//return TW_KEY_PAUSE;
	case eKey::KEY_ESCAPE:
		return TW_KEY_ESCAPE;
	case eKey::KEY_DELETE:
		return TW_KEY_DELETE;
	case eKey::KEY_UP:
		return TW_KEY_UP;
	case eKey::KEY_DOWN:
		return TW_KEY_DOWN;
	case eKey::KEY_RIGHT:
		return TW_KEY_RIGHT;
	case eKey::KEY_LEFT:
		return TW_KEY_LEFT;
	case eKey::KEY_INSERT:
		return TW_KEY_INSERT;
	case eKey::KEY_HOME:
		return TW_KEY_HOME;
	case eKey::KEY_END:
		return TW_KEY_END;
	case eKey::KEY_PAGE_UP:
		return TW_KEY_PAGE_UP;
	case eKey::KEY_PAGE_DOWN:
		return TW_KEY_PAGE_DOWN;
	case eKey::KEY_F1:
		return TW_KEY_F1;
	case eKey::KEY_F2:
		return TW_KEY_F2;
	case eKey::KEY_F3:
		return TW_KEY_F3;
	case eKey::KEY_F4:
		return TW_KEY_F4;
	case eKey::KEY_F5:
		return TW_KEY_F5;
	case eKey::KEY_F6:
		return TW_KEY_F6;
	case eKey::KEY_F7:
		return TW_KEY_F7;
	case eKey::KEY_F8:
		return TW_KEY_F8;
	case eKey::KEY_F9:
		return TW_KEY_F9;
	case eKey::KEY_F10:
		return TW_KEY_F10;
	case eKey::KEY_F11:
		return TW_KEY_F11;
	case eKey::KEY_F12:
		return TW_KEY_F12;
	default:
		//dg::DebugMessage("Unimplemented to ATB key");
		break;
	}

	return TW_KEY_LAST;
}

const unique_ptr<UI>& UI::Instance() {
	if(!m_Instance) {
		m_Instance = unique_ptr<UI>(new UI());
		if(!m_Instance->Initialize()) {
			Release();
		}
	}
	return m_Instance;
}

UI::UI() {
	m_SceneBar = nullptr;
	m_ActiveSceneObjectBar = nullptr;
	m_ActiveSceneObject = "";
	m_DrawUI = true;
}

UI::~UI() {}

void UI::Release() {
	if(m_Instance==nullptr)
		return;

	// Release all bars.
	Instance()->ReleaseSceneBar();
	Instance()->ReleaseActiveSceneObjectBar();
	Instance()->ReleaseActiveMaterialBar();
	Instance()->ReleaseEnvironmentBar();
	Instance()->ReleaseTexturesBar();
	Instance()->ReleaseMaterialsBar();

	// AntTweakBar.
	TwTerminate();

	m_Instance = nullptr;
}

bool UI::Initialize() {
	if(TwInit(TW_OPENGL_CORE, NULL)) {
		TwWindowSize(Configuration::Instance()->GetScreenWidth(), Configuration::Instance()->GetScreenHeigth());

		// Global AntTweakBar behaviour.
		TwDefine(" GLOBAL contained=true ");

		// Types definition.
		TwStructMember Vec3Members[] = {
			{"x", TW_TYPE_FLOAT, sizeof(float)*0, ""},
			{"y", TW_TYPE_FLOAT, sizeof(float)*1, ""},
			{"z", TW_TYPE_FLOAT, sizeof(float)*2, ""}
		};
		TW_TYPE_VEC3 = TwDefineStruct("vec3", Vec3Members, 3, sizeof(vec3), NULL, NULL);

		TwStructMember Vec2Members[] = {
			{"x", TW_TYPE_FLOAT, sizeof(float)*0, ""},
			{"y", TW_TYPE_FLOAT, sizeof(float)*1, ""}
		};
		TW_TYPE_VEC2 = TwDefineStruct("vec2", Vec2Members, 2, sizeof(vec2), NULL, NULL);

		TwEnumVal lights[] = {
			{DIRECTIONAL_LIGHT, "Directional"},
			{POINT_LIGHT, "Point"},
			{SPOT_LIGHT, "Spot"}
		};
		TW_TYPE_LIGHT = TwDefineEnum("LightType", lights, ARRAY_SIZE_IN_ELEMENTS(lights));

		TwEnumVal wraps[] = {
			{WRAP_REPEAT, "REPEAT"},
			{WRAP_CLAMP_TO_EDGE, "CLAMP_TO_EDGE"},
			{WRAP_CLAMP_TO_BORDER, "CLAMP_TO_BORDER"},
			{WRAP_MIRRORED_REPEAT, "MIRRORED_REPEAT"}
		};
		TW_TYPE_WRAP = TwDefineEnum("WrapType", wraps, ARRAY_SIZE_IN_ELEMENTS(wraps));

		TwEnumVal filters[] = {
			{FILTER_NEAREST, "NEAREST"},
			{FILTER_LINEAR, "LINEAR"},
			{FILTER_NEAREST_MIPMAP_NEAREST, "NEAREST_MIPMAP_NEAREST"},
			{FILTER_LINEAR_MIPMAP_NEAREST, "LINEAR_MIPMAP_NEAREST"},
			{FILTER_NEAREST_MIPMAP_LINEAR, "NEAREST_MIPMAP_LINEAR"},
			{FILTER_LINEAR_MIPMAP_LINEAR, "LINEAR_MIPMAP_LINEAR"}
		};
		TW_TYPE_FILTER = TwDefineEnum("FilterType", filters, ARRAY_SIZE_IN_ELEMENTS(filters));

		TwEnumVal fogs[] = {
			{LINEAR_FOG, "LINEAR"},
			{EXPONENTIAL_FOG, "EXPONENTIAL"},
			{EXPONENTIAL_SQUARED_FOG, "EXPONENTIAL_SQUARED"},
			{ALTITUDE_FOG, "ALTITUDE"}
		};
		TW_TYPE_FOG = TwDefineEnum("FogType", fogs, ARRAY_SIZE_IN_ELEMENTS(fogs));

		TwEnumVal blendFuncs[] = {
			{BF_ZERO, "ZERO"},
			{BF_ONE, "ONE"},
			{BF_SRC_COLOR, "SRC_COLOR"},
			{BF_ONE_MINUS_SRC_COLOR, "ONE_MINUS_SRC_COLOR"},
			{BF_DST_COLOR, "DST_COLOR"},
			{BF_ONE_MINUS_DST_COLOR, "ONE_MINUS_DST_COLOR"},
			{BF_SRC_ALPHA, "SRC_ALPHA"},
			{BF_ONE_MINUS_SRC_ALPHA, "ONE_MINUS_SRC_ALPHA"},
			{BF_DST_ALPHA, "DST_ALPHA"},
			{BF_ONE_MINUS_DST_ALPHA, "ONE_MINUS_DST_ALPHA"},
			{BF_CONSTANT_COLOR, "CONSTANT_COLOR"},
			{BF_ONE_MINUS_CONSTANT_COLOR, "ONE_MINUS_CONSTANT_COLOR"},
			{BF_CONSTANT_ALPHA, "CONSTANT_ALPHA"},
			{BF_ONE_MINUS_CONSTANT_ALPHA, "ONE_MINUS_CONSTANT_ALPH"},
			{BF_SRC_ALPHA_SATURATE, "SRC_ALPHA_SATURATE"},
			{BF_SRC1_COLOR, "SRC1_COLOR"},
			{BF_ONE_MINUS_SRC1_COLOR, "ONE_MINUS_SRC1_COLOR"},
			{BF_SRC1_ALPHA, "SRC1_ALPHA"},
			{BF_ONE_MINUS_SRC1_ALPHA, "ONE_MINUS_SRC1_ALPHA"},
		};
		TW_TYPE_BLEND_FUNC = TwDefineEnum("BlendFuncType", blendFuncs, ARRAY_SIZE_IN_ELEMENTS(blendFuncs));

		TwEnumVal blendEquations[] = {
			{BE_FUNC_ADD, "BE_FUNC_ADD"},
			{BE_FUNC_SUBTRACT, "BE_FUNC_SUBTRACT"},
			{BE_FUNC_REVERSE_SUBTRACT, "BE_FUNC_REVERSE_SUBTRACT"},
			{BE_MIN, "BE_MIN"},
			{BE_MAX, "BE_MAX"}
		};
		TW_TYPE_BLEND_EQUATION = TwDefineEnum("BlendEquationType", blendEquations, ARRAY_SIZE_IN_ELEMENTS(blendEquations));

		TwEnumVal renderModes[] = {
			{OPAQUE_RENDER, "OPAQUE"},
			{TRANSPARENT_RENDER, "TRANSPARENT"}
		};
		TW_TYPE_RENDER_MODE = TwDefineEnum("RenderType", renderModes, ARRAY_SIZE_IN_ELEMENTS(renderModes));

		TwEnumVal shadowsType[] = {
			{NO_SHADOW, "NO_SHADOW"},
			{HARD_SHADOW, "HARD_SHADOW"},
			{SOFT_SHADOW, "SOFT_SHADOW"}
		};
		TW_TYPE_SHADOW = TwDefineEnum("ShadowsType", shadowsType, ARRAY_SIZE_IN_ELEMENTS(shadowsType));

		TwEnumVal projectionsType[] = {
			{ORTHOGRAPHIC_PROJECTION, "ORTHOGRAPHIC"},
			{PERSPECTIVE_PROJECTION, "PERSPECTIVE"}
		};
		TW_TYPE_CAMERA_PROJECTION = TwDefineEnum("CameraProjectionsType", projectionsType, ARRAY_SIZE_IN_ELEMENTS(projectionsType));

		return true;
	}
	return false;
}

void UI::RenderUI() {
	// Draw the tweak bar(s).
	if(m_Instance==nullptr)
		return;

	if(!Instance()->m_DrawUI)
		return;

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// If active, refresh scene object bar.
	if(Instance()->m_ActiveSceneObjectBar)
		TwRefreshBar(Instance()->m_ActiveSceneObjectBar);

	TwDraw();
}

void UI::DrawUI(const bool& value) {
	Instance()->m_DrawUI = value;
}

void UI::InitializeSceneBar() {
	// Initializing hierarchy bar.
	if(!m_SceneBar) {
		m_SceneBar = TwNewBar(ApplicationManager::CurrentScene()->SceneName().c_str());
	}

	for(unsigned int i = 0; i<SceneObject::Root()->Children().size(); i++) {
		InitializeSceneBar(SceneObject::Root()->Children()[i], 0);
	}
}

void UI::InitializeSceneBar(const SceneObject* sceneObject, int level) {

	// Initializing hierarchy bar.
	if(!m_SceneBar) {
		m_SceneBar = TwNewBar(ApplicationManager::CurrentScene()->SceneName().c_str());
	}

	static string space = "  ";
	string nodeName = "";
	for(int i = 0; i<level; i++) {
		nodeName += space;
	}
	nodeName += "-";
	nodeName += sceneObject->Id();

	TwAddButton(m_SceneBar, nodeName.c_str(), SceneObjectSelectedCallback, (void*) sceneObject, "");

	for(unsigned int i = 0; i<sceneObject->Children().size(); i++) {
		InitializeSceneBar(sceneObject->Children()[i], level+1);
	}
}

void UI::UpdateSceneBar() {
	if(m_SceneBar) {
		ReleaseSceneBar();
		InitializeSceneBar(SceneObject::Root().get());
	}
}

void UI::ReleaseSceneBar() {
	if(m_SceneBar) {
		TwDeleteBar(m_SceneBar);
		m_SceneBar = nullptr;
	}
}

void UI::SetActiveSceneObject(const string& idSceneObject) {
	if(m_ActiveSceneObject!=idSceneObject) {
		SceneObject* sceneObject = SceneObject::FindSceneObjectById(idSceneObject);
		if(sceneObject) {
			ReleaseActiveSceneObjectBar();
			InitializeActiveSceneObjectBar(sceneObject);
		}
		m_ActiveSceneObject = idSceneObject;
	}
}

void UI::InitializeActiveSceneObjectBar(SceneObject* sceneObject) {
	// Create the bar only once (When the same object is selected repetedly).
	if(m_ActiveSceneObjectBar) return;

	m_ActiveSceneObjectBar = TwNewBar(sceneObject->Id().c_str());

	sceneObject->DrawUI(m_ActiveSceneObjectBar);
}

void UI::ReleaseActiveSceneObjectBar() {
	if(m_ActiveSceneObjectBar) {
		// Release the scene object bar.
		TwDeleteBar(m_ActiveSceneObjectBar);
		// Release the material bar.
		//ReleaseActiveMaterialBar();
		m_ActiveSceneObjectBar = nullptr;
	}
}

void UI::SetActiveMaterial(Material* material) {
	if(m_ActiveMaterial!=material) {
		ReleaseActiveMaterialBar();
		InitializeActiveMaterialBar(material);
		m_ActiveMaterial = material;
	}
}

void UI::InitializeActiveMaterialBar(Material* material) {
	if(!m_ActiveMaterialBar) {
		m_ActiveMaterialBar = TwNewBar(material->GetName().c_str());
		material->DrawUI(m_ActiveMaterialBar);
	}
}

void UI::ReleaseActiveMaterialBar() {
	if(m_ActiveMaterialBar) {
		TwDeleteBar(m_ActiveMaterialBar);
		m_ActiveMaterialBar = nullptr;
	}
}

void UI::SetActiveTexture(Texture* texture) {
	if(m_ActiveTexture!=texture) {
		ReleaseActiveTextureBar();
		InitializeActiveTextureBar(texture);
		m_ActiveTexture = texture;
	}
}

void UI::InitializeActiveTextureBar(Texture* texture) {
	if(!m_ActiveTextureBar) {
		m_ActiveTextureBar = TwNewBar(texture->GetFileName().c_str());
		texture->DrawUI(m_ActiveTextureBar);
	}
}

void UI::ReleaseActiveTextureBar() {
	if(m_ActiveTextureBar) {
		TwDeleteBar(m_ActiveTextureBar);
		m_ActiveTextureBar = nullptr;
	}
}

void UI::InitializeEnvironmentBar() {
	char parameter[128];
	memset(parameter, 0, sizeof(parameter));

	if(!m_EnvironmentBar) {
		m_EnvironmentBar = TwNewBar("Environment");

		// Bar position.
		int posX = 10;
		int posY = Configuration::Instance()->GetScreenHeigth()/2;
		SNPRINTF(parameter, sizeof(parameter), "Environment position='%d %d' ", posX, posY);
		TwDefine(parameter);

		// Iconified.
		TwDefine("Environment iconified=true");

		Environment::Instance()->DrawUI(m_EnvironmentBar);
	}
}

void UI::ReleaseEnvironmentBar() {
	if(m_EnvironmentBar) {
		TwDeleteBar(m_EnvironmentBar);
		m_EnvironmentBar = nullptr;
	}
}

void UI::InitializeTexturesBar() {
	char parameter[128];
	memset(parameter, 0, sizeof(parameter));

	if(!m_TexturesBar) {
		m_TexturesBar = TwNewBar("Textures");

		// Bar position.
		int posX = 20+10;
		int posY = 20+Configuration::Instance()->GetScreenHeigth()/2;
		SNPRINTF(parameter, sizeof(parameter), "Textures position='%d %d' ", posX, posY);
		TwDefine(parameter);

		// Iconified.
		TwDefine("Textures iconified=true");

		Texture::DrawUITextures(m_TexturesBar, DataManager::GetTexturesMap());
	}
}

void UI::ReleaseTexturesBar() {
	if(m_TexturesBar) {
		TwDeleteBar(m_TexturesBar);
		m_TexturesBar = nullptr;
	}
}

void UI::InitializeMaterialsBar() {
	char parameter[128];
	memset(parameter, 0, sizeof(parameter));

	if(!m_MaterialsBar) {
		m_MaterialsBar = TwNewBar("Materials");

		// Bar position.
		int posX = 40+10;
		int posY = 40+Configuration::Instance()->GetScreenHeigth()/2;
		SNPRINTF(parameter, sizeof(parameter), "Materials position='%d %d' ", posX, posY);
		TwDefine(parameter);

		// Iconified.
		TwDefine("Materials iconified=true");

		Material::DrawUIMaterials(m_MaterialsBar, DataManager::GetMaterialsMap());
	}
}

void UI::ReleaseMaterialsBar() {
	if(m_MaterialsBar) {
		TwDeleteBar(m_MaterialsBar);
		m_MaterialsBar = nullptr;
	}
}
