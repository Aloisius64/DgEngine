////////////////////////////////////////////////////////////////////////////////
// Filename: TextRenderer.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "TextRenderer.h"
#include "Configuration.h"
#include "DataManager.h"
#include "Glossary.h"
#include "Utils.h"

//////////
// LIBS //
//////////
#pragma comment (lib, "freetype.lib")    /* link freetype lib */

////////////////
// NAMESPACES //
////////////////
using namespace dg;


unique_ptr<TextRenderer> TextRenderer::m_Instance = nullptr;

TextRenderer* TextRenderer::Instance() {
	if(!m_Instance) {
		TextRendererDesc desc;
		desc.fontPath = DEFAULT_FONT;
		desc.width = 0;
		desc.height = 48;
		m_Instance = unique_ptr<TextRenderer>(new TextRenderer(desc));
	}
	return m_Instance.get();
}

TextRenderer* TextRenderer::Instance(const TextRendererDesc& textRendererDesc) {
	if(!m_Instance) {
		m_Instance = unique_ptr<TextRenderer>(new TextRenderer(textRendererDesc));
	}
	return m_Instance.get();
}

void TextRenderer::Release() {
	m_Instance = nullptr;
}

void TextRenderer::Render() {
	if(m_Instance) {
		GLint value = 0;
		glGetIntegerv(GL_BLEND, &value);

		if(value!=GL_TRUE) {
			glEnable(GL_BLEND);
		}
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDisable(GL_DEPTH_TEST);

		m_Instance->RenderMessages();
		
		glEnable(GL_DEPTH_TEST);

		if(value!=GL_TRUE) {
			glDisable(GL_BLEND);
		}
	}
}

TextRenderer::TextRenderer(const TextRendererDesc& textRendererDesc) {
	m_FontPath = textRendererDesc.fontPath;
	m_Shader = shared_ptr<Shader>(DataManager::GetShader(SHADER_TEXT_RENDERER));

	// FreeType
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if(FT_Init_FreeType(&ft))
		dg::DebugConsoleMessage("ERROR::FREETYPE: Could not init FreeType Library\n");

	// Load font as face
	FT_Face face;
	if(FT_New_Face(ft, m_FontPath.c_str(), 0, &face))
		dg::DebugConsoleMessage("ERROR::FREETYPE: Failed to load font\n");

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, textRendererDesc.width, textRendererDesc.height);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Load first 128 characters of ASCII set
	for(GLubyte c = 0; c<128; c++) {
		// Load character glyph 
		if(FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			dg::DebugConsoleMessage("ERROR::FREETYTPE: Failed to load Glyph\n");
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
			);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		m_Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*6*4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

TextRenderer::~TextRenderer() {
	if(glIsVertexArray(m_VAO)) {
		glDeleteVertexArrays(1, &m_VAO);
	}

	if(glIsBuffer(m_VBO)) {
		glDeleteBuffers(1, &m_VBO);
	}

	//m_Shader = nullptr;
}

void TextRenderer::RenderText(const string& text, const GLfloat& x, const GLfloat& y, const GLfloat& scale, const vec3& color) {
	m_MessageQueue.push(Message(text, x, y, scale, color));
}

void TextRenderer::RenderMessages() {
	while(!m_MessageQueue.empty()) {
		Message message = m_MessageQueue.front();
		m_MessageQueue.pop();

		// Activate corresponding render state	
		m_Shader->SetActive();
		glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(Configuration::Instance()->GetScreenWidth()), 0.0f, static_cast<GLfloat>(Configuration::Instance()->GetScreenHeigth()));
		m_Shader->SetFloatMatrix4("projection", projection);
		m_Shader->SetFloatVector3("textColor", message.color);

		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(m_VAO);

		// Iterate through all characters
		std::string::const_iterator c;
		for(c = message.text.begin(); c!=message.text.end(); c++) {
			Character ch = m_Characters[*c];

			GLfloat xpos = message.x+ch.Bearing.x * message.scale;
			GLfloat ypos = message.y-(ch.Size.y-ch.Bearing.y) * message.scale;

			GLfloat w = ch.Size.x * message.scale;
			GLfloat h = ch.Size.y * message.scale;
			// Update VBO for each character
			GLfloat vertices[6][4] = {
				{xpos, ypos+h, 0.0, 0.0},
				{xpos, ypos, 0.0, 1.0},
				{xpos+w, ypos, 1.0, 1.0},

				{xpos, ypos+h, 0.0, 0.0},
				{xpos+w, ypos, 1.0, 1.0},
				{xpos+w, ypos+h, 1.0, 0.0}
			};
			// Render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			// Update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// Render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			message.x += (ch.Advance>>6) * message.scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
