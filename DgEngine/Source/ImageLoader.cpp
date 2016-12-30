////////////////////////////////////////////////////////////////////////////////
// Filename: ImageLoader.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "ImageLoader.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


unique_ptr<ImageLoader> ImageLoader::m_Instance = nullptr;

ImageLoader::ImageLoader() {
	ilInit();
}

ImageLoader::~ImageLoader() {

}

const unique_ptr<ImageLoader>& ImageLoader::Instance() {
	if(!m_Instance) {
		m_Instance = unique_ptr<ImageLoader>(new ImageLoader());
	}
	return m_Instance;
}

bool ImageLoader::LoadImg(std::string fileName) {
	ILboolean success;
	ilGenImages(1, &Instance()->m_Image);
	ilBindImage(Instance()->m_Image);
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	success = ilLoadImage((ILstring) fileName.c_str());
	//ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		
	if(!success) {
		ilDeleteImages(1, &Instance()->m_Image);
		return false;
	}

	return true;
}

void ImageLoader::FreeCPUMemory() {
	ilDeleteImages(1, &Instance()->m_Image);
}

GLint ImageLoader::GetImageInternalFormat() {
	return ilGetInteger(IL_IMAGE_BPP);
}

GLsizei ImageLoader::GetImageWidth() {
	return ilGetInteger(IL_IMAGE_WIDTH);
}

GLsizei ImageLoader::GetImageHeight() {
	return ilGetInteger(IL_IMAGE_HEIGHT);
}

GLenum ImageLoader::GetImageFormat() {
	return ilGetInteger(IL_IMAGE_FORMAT);
}

GLvoid* ImageLoader::GetImageData() {
	return ilGetData();
}

void ImageLoader::Release() {
	m_Instance = nullptr;
}
