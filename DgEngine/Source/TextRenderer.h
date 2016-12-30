////////////////////////////////////////////////////////////////////////////////
// Filename: TextRenderer.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <GL/glew.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Shader.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	struct TextRendererDesc {
		string fontPath;
		unsigned int width;
		unsigned int height;

		TextRendererDesc() {
			fontPath = "";
			width = 0;
			height = 0;
		}
	};

	/// Holds all state information relevant to a character as loaded using FreeType
	struct Character {
		GLuint TextureID;	// ID handle of the glyph texture
		glm::ivec2 Size;	// Size of glyph
		glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
		GLuint Advance;		// Horizontal offset to advance to next glyph
	};
	
	typedef std::map<GLchar, Character> CharactersMap;

	class TextRenderer {
	public:
		static TextRenderer* Instance(const TextRendererDesc&);
		static TextRenderer* Instance();
		static void Release();
		static void Render();

	public:
		virtual ~TextRenderer();

		void RenderText(const string&, const GLfloat&, const GLfloat&, const GLfloat&, const vec3&);
		
	private:
		TextRenderer(const TextRendererDesc&);

		void RenderMessages();

		struct Message {
			string text;
			GLfloat x;
			GLfloat y;
			GLfloat scale;
			vec3 color;

			Message(string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color) {
				this->text = text;
				this->x = x;
				this->y = y;
				this->scale = scale;
				this->color = color;
			}
		};

	private:
		static unique_ptr<TextRenderer>		m_Instance;
		shared_ptr<Shader>					m_Shader;
		string								m_FontPath;
		unsigned int						m_Width;
		unsigned int						m_Height;
		CharactersMap						m_Characters;
		GLuint								m_VAO;
		GLuint								m_VBO;
		queue<Message>						m_MessageQueue;
	};
}
