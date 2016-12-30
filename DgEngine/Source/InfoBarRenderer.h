////////////////////////////////////////////////////////////////////////////////
// Filename: InfoBarRenderer.h
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
	class InfoBarRenderer {
	public:
		static InfoBarRenderer* Instance();
		static void Release();
		static void Render();

	public:
		virtual ~InfoBarRenderer();

		void RenderInfoBar(const string&, const eInfoBarOrientation&, const int&, const int&, const int&, const int&, const string&, const string&, const vec4&, const vec4&);
		
	private:
		InfoBarRenderer();

		void RenderBars();

	private:
		struct InfoBarDesc {
			string name;
			eInfoBarOrientation orientation;
			int xPosition;
			int yPosition;
			int width;
			int height;
			string minValue;
			string maxValue;
			vec4 minColor;
			vec4 maxColor;

			InfoBarDesc(const string& name, const eInfoBarOrientation& orientation,
				const int& xPos, const int& yPos, 
				const int& width, const int& height, 
				const string& minValue, const string& maxValue,
				const vec4& minColor, const vec4& maxColor) {
				this->name = name;
				this->orientation = orientation;
				this->xPosition = xPos;
				this->yPosition = yPos;
				this->width = width;
				this->height = height;
				this->minValue = minValue;
				this->maxValue = maxValue;
				this->minColor = minColor;
				this->maxColor = maxColor;
			}
		};

	private:
		static unique_ptr<InfoBarRenderer>	m_Instance;
		shared_ptr<Shader>					m_Shader;
		GLuint								m_VAO;
		GLuint								m_VertexBuffer;
		GLuint								m_ColorBuffer;
		queue<InfoBarDesc>					m_BarsQueue;
	};
}
