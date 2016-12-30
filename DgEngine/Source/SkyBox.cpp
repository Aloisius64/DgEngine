////////////////////////////////////////////////////////////////////////////////
// Filename: SkyBox.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////
#include <cstdlib>
#include <fstream>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "SkyBox.h"
#include "Camera.h"
#include "Common.h"
#include "DataManager.h"
#include "Environment.h"
#include "Glossary.h"
#include "Transform.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


SkyBox::SkyBox() {
	m_CubemapTex = nullptr;
	m_Transform = nullptr;
	m_Mesh = nullptr;
	m_Shader = nullptr;
}

SkyBox::~SkyBox() {
	m_CubemapTex = nullptr;
	m_Transform = nullptr;
	m_Mesh = nullptr;
	m_Shader = nullptr;
}

bool SkyBox::Initialize(const string& xPositiveFilename,
	const string& xNegativeFilename,
	const string& yPositiveFilename,
	const string& yNegativeFilename,
	const string& zPositiveFilename,
	const string& zNegativeFilename) {
	
	m_CubemapTex = unique_ptr<CubemapTexture>(new CubemapTexture(xPositiveFilename,
		xNegativeFilename,
		yPositiveFilename,
		yNegativeFilename,
		zPositiveFilename,
		zNegativeFilename));

	if(!m_CubemapTex->Load()) {
		return false;
	}

	// Enable seamless cube-map sampling.
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Initialize shader.
	m_Shader = shared_ptr<Shader>(DataManager::GetShader(SHADER_SKYBOX));

	// Load sphere obj.
	m_Mesh = DataManager::Instance()->GetMesh(SKYBOX_SPHERE_MESH_PATH);

	// Create a Transform component.
	m_Transform = unique_ptr<Transform>(new Transform(NULL, vec_zero, vec_zero, vec_one*200.0f));

	return true;
}

void SkyBox::Render() {
	// Set skybox shader active
	m_Shader->SetActive();

	// Update component
	m_Transform->Update();

	// Set shader parameters
	m_Transform->SetPosition(Camera::ActiveCamera()->GetPosition());
	m_Shader->SetFloatMatrix4("MVPMatrix", m_Transform->GetWorldViewProjectionMatrix());
	m_Shader->SetInt("CubemapTexture", COLOR_TEXTURE_UNIT_INDEX);
	// Gamma.
	m_Shader->SetFloat("Gamma", Environment::Instance()->GetGamma());
	// HDR.
	m_Shader->SetInt("ActiveHDR", Environment::Instance()->IsHdrActive());
	m_Shader->SetInt("ActiveOIT", Environment::Instance()->IsOITActive());

	GLint oldCullFaceMode;
	glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
	GLint oldDepthFuncMode;
	glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);

	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);
	
	m_CubemapTex->Bind(COLOR_TEXTURE_UNIT);

	m_Mesh->Render(0);

	m_CubemapTex->Unbind(COLOR_TEXTURE_UNIT);

	glCullFace(oldCullFaceMode);
	glDepthFunc(oldDepthFuncMode);
}

const unique_ptr<CubemapTexture>& SkyBox::GetCubemapTex() const {
	return m_CubemapTex;
}
