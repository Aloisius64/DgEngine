////////////////////////////////////////////////////////////////////////////////
// Filename: LogoScene.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "ApplicationScene.h"
#include "Common.h"
#include "Shader.h"
#include "System.h"
#include "Texture.h"
#include "TimeClock.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


class LogoScene : public ApplicationScene {
public:
	LogoScene(string nameScene)
		: ApplicationScene(nameScene) {
		m_Shader = nullptr;
		m_TextureLogo = nullptr;
	}

	virtual ~LogoScene() {
		m_Shader = nullptr;
		m_TextureLogo = nullptr;
	}

	virtual bool Initialize() {
		// Generating the VAO.
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		// Get the shader.
		m_Shader = DataManager::GetShader(SHADER_LOGO);

		// Get the logo texture.
		TextureDesc logoDesc;
		logoDesc.filePath = TEXTURE_LOGO_PATH;
		logoDesc.wrapS = eTextureWrapParameter::WRAP_CLAMP_TO_BORDER;
		logoDesc.wrapT = eTextureWrapParameter::WRAP_CLAMP_TO_BORDER;
		logoDesc.magFilter = eTextureFilterParameter::FILTER_LINEAR;
		logoDesc.minFilter = eTextureFilterParameter::FILTER_LINEAR;
		m_TextureLogo = DataManager::TryGetTexture(logoDesc);

		return true;
	}

	virtual bool Frame() {
		glGetIntegerv(GL_BLEND, &value);

		dg::System::Instance()->ClearBuffers(color_black);

		// Render the logo.
		m_Shader->SetActive();
		m_Shader->SetInt("imageLogo", 0);
		m_TextureLogo->Bind(GL_TEXTURE0);

		glDisable(GL_DEPTH_TEST);

		if(value!=GL_TRUE) {
			glEnable(GL_BLEND);
		}

		glBindVertexArray(m_VAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		if(value!=GL_TRUE) {
			glDisable(GL_BLEND);
		}

		glEnable(GL_DEPTH_TEST);

		dg::System::Instance()->SwapBuffers();

		return true;
	}

	virtual void Release() {
		DELETE_VERTEX_ARRAY(m_VAO);
	}

private:
	GLint				value;
	GLuint				m_VAO;
	shared_ptr<Shader>	m_Shader;
	shared_ptr<Texture>	m_TextureLogo;
};
