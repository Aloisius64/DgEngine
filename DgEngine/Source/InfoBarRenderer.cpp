////////////////////////////////////////////////////////////////////////////////
// Filename: InfoBarRenderer.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "InfoBarRenderer.h"
#include "Configuration.h"
#include "DataManager.h"
#include "TextRenderer.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


vec4 vertices[4];
vec4 colors[4];
#define OFFSET_BAR_STRING 12

unique_ptr<InfoBarRenderer> InfoBarRenderer::m_Instance = nullptr;

InfoBarRenderer* InfoBarRenderer::Instance() {
	if(!m_Instance) {
		m_Instance = unique_ptr<InfoBarRenderer>(new InfoBarRenderer());
	}
	return m_Instance.get();
}

void InfoBarRenderer::Release() {
	m_Instance = nullptr;
}

void InfoBarRenderer::Render() {
	if(m_Instance) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDisable(GL_DEPTH_TEST);

		m_Instance->RenderBars();

		glEnable(GL_DEPTH_TEST);
	}
}

InfoBarRenderer::InfoBarRenderer() {
	m_Shader = shared_ptr<Shader>(DataManager::GetShader(SHADER_INFOBAR_RENDERER));

	// Configure VAO/VBO for texture quads.
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VertexBuffer);
	glGenBuffers(1, &m_ColorBuffer);
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_ColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

InfoBarRenderer::~InfoBarRenderer() {
	DELETE_VERTEX_ARRAY(m_VAO);
	DELETE_BUFFER(m_VertexBuffer);
	DELETE_BUFFER(m_ColorBuffer);
}

void InfoBarRenderer::RenderInfoBar(const string& name, const eInfoBarOrientation& orientation, const int& xPos, const int& yPos, const int& width, const int& height, const string& minValue, const string& maxValue, const vec4& minColor, const vec4& maxColor) {
	m_BarsQueue.push(InfoBarDesc(name, orientation, xPos, yPos, width, height, minValue, maxValue, minColor, maxColor));
}

void InfoBarRenderer::RenderBars() {
	while(!m_BarsQueue.empty()) {
		InfoBarDesc bar = m_BarsQueue.front();
		m_BarsQueue.pop();

		m_Shader->SetActive();
		glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(Configuration::Instance()->GetScreenWidth()), 0.0f, static_cast<GLfloat>(Configuration::Instance()->GetScreenHeigth()));
		m_Shader->SetFloatMatrix4("projection", projection);

		vertices[0] = vec4(bar.xPosition, bar.yPosition, 0.0, 0.0);
		vertices[1] = vec4(bar.xPosition+bar.width, bar.yPosition, 1.0, 0.0);
		vertices[2] = vec4(bar.xPosition, bar.yPosition+bar.height, 0.0, 1.0);
		vertices[3] = vec4(bar.xPosition+bar.width, bar.yPosition+bar.height, 1.0, 1.0);

		if(bar.orientation==eInfoBarOrientation::INFO_BAR_HORIZONTAL) {
			colors[0] = vec4(bar.minColor);
			colors[1] = vec4(bar.maxColor);
			colors[2] = vec4(bar.minColor);
			colors[3] = vec4(bar.maxColor);
			
			TextRenderer::Instance()->RenderText(bar.name, 
				(float) bar.xPosition, (float) bar.yPosition+bar.height+OFFSET_BAR_STRING,
				0.5f, vec_one);

			TextRenderer::Instance()->RenderText(bar.minValue,
				(float) bar.xPosition, (float) bar.yPosition-OFFSET_BAR_STRING*2.0f,
				0.5f, vec_one);

			TextRenderer::Instance()->RenderText(bar.maxValue,
				(float) bar.xPosition+bar.width, (float) bar.yPosition-OFFSET_BAR_STRING*2.0f,
				0.5f, vec_one);
		} else {
			colors[0] = vec4(bar.minColor);
			colors[1] = vec4(bar.minColor);
			colors[2] = vec4(bar.maxColor);
			colors[3] = vec4(bar.maxColor);
		
			TextRenderer::Instance()->RenderText(bar.name,
				(float) bar.xPosition, (float) bar.yPosition+bar.height+OFFSET_BAR_STRING,
				0.5f, vec_one);

			TextRenderer::Instance()->RenderText(bar.minValue,
				(float) bar.xPosition+bar.width+OFFSET_BAR_STRING*2.0f, (float) bar.yPosition,
				0.5f, vec_one);

			TextRenderer::Instance()->RenderText(bar.maxValue,
				(float) bar.xPosition+bar.width+OFFSET_BAR_STRING*2.0f, (float) bar.yPosition+bar.height-OFFSET_BAR_STRING,
				0.5f, vec_one);
		}

		glBindVertexArray(m_VAO);

		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, m_ColorBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(colors), colors);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		// Render quad
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindVertexArray(0);
	}
}
