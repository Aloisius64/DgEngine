////////////////////////////////////////////////////////////////////////////////
// Filename: CubemapTexture.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <string>
#include <vector>
#include <GL/glew.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class CubemapTexture {
	public:
		CubemapTexture(const string&,
			const string&,
			const string&,
			const string&,
			const string&,
			const string&);
		virtual ~CubemapTexture();

		bool Load();
		void Bind(GLenum) const;
		void Unbind(GLenum) const;
		const vector<string>& GetTextureNames() const;

	private:
		vector<string>	m_FileNames;
		GLuint			m_TextureObj;
	};
}
