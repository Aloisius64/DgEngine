////////////////////////////////////////////////////////////////////////////////
// Filename: ImageLoader.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <memory>
#include <string>
#include <IL/il.h>
#include <GL/glew.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class ImageLoader {
	public:
		static const unique_ptr<ImageLoader>& Instance();
		static bool LoadImg(string);
		static void FreeCPUMemory();
		static void Release();
		static GLint GetImageInternalFormat();
		static GLsizei ImageLoader::GetImageWidth();
		static GLsizei ImageLoader::GetImageHeight();
		static GLenum ImageLoader::GetImageFormat();
		static GLvoid* ImageLoader::GetImageData();

	public:
		virtual ~ImageLoader();

	private:
		ImageLoader();

	private:
		static unique_ptr<ImageLoader>	m_Instance;
		ILuint							m_Image;
	};
}
