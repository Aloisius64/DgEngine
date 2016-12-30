////////////////////////////////////////////////////////////////////////////////
// Filename: Utils.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <fstream>
#include <iostream>
#include <string>
#include <GL/glew.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Common.h"
#include "Mathematics.h"

#define FREE_PTR(pointer) if (pointer) { delete pointer; pointer = nullptr; }
#define ZERO_MEM(buffer) memset(buffer, 0, sizeof(buffer))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define INVALID_UNIFORM_LOCATION 0xffffffff
#define GLCheckError() (glGetError() == GL_NO_ERROR)
#define GLDebugError() dg::InfoGLError(glGetError());

#ifdef WIN32
#define SNPRINTF _snprintf_s
#define RANDOM rand
#define SRANDOM srand((unsigned)time(NULL))
#else
#define SNPRINTF snprintf
#define RANDOM random
#define SRANDOM srandom(getpid())
#endif

#define DELETE_FRAMEBUFFER(x) if(glIsFramebuffer(x)) glDeleteFramebuffers(1, &x); x = 0;
#define DELETE_TEXTURE(x) if(glIsTexture(x)) glDeleteTextures(1, &x); x = 0;
#define DELETE_BUFFER(x) if(glIsBuffer(x)) glDeleteBuffers(1, &x); x = 0;
#define DELETE_BUFFERS(s, x) if(s>0 && x && glIsBuffer(x[0])) glDeleteBuffers(s, x);
#define DELETE_VERTEX_ARRAY(x) if(glIsVertexArray(x)) glDeleteVertexArrays(1, &x); x = 0;
#define DELETE_QUERY(x) if(glIsQuery(x)) glDeleteQueries(1, &x); x = 0;


namespace dg {
	bool ReadFile(const char*, std::string&);
	void DebugConsoleMessage(const char*);
	glm::vec3 DirectionFromRotation(const glm::vec3&, const glm::vec3&);
	glm::vec3 DirectionFromRotation(const glm::vec3&, const glm::quat&);
	glm::vec3 ForwardDirectionFromRotation(const glm::vec3&);
	glm::vec3 UpDirectionFromRotation(const glm::vec3&);
	glm::vec3 RightDirectionFromRotation(const glm::vec3&);
	glm::vec3 ForwardDirectionFromRotation(const glm::quat&);
	glm::vec3 UpDirectionFromRotation(const glm::quat&);
	glm::vec3 RightDirectionFromRotation(const glm::quat&);
	void GetScreenResolution(int& width, int& height);
	std::wstring stringToLPCWSTR(const std::string& s);
	float RandomFloat();
	bool CheckGLError();
	void InfoGLError(unsigned int);
	void InfoGLFramebufferError(unsigned int);
	eTextureType ConvertToTextureType(const std::string&);
	eTextureWrapParameter ConvertToTextureWrap(const std::string&);
	eTextureFilterParameter ConvertToTextureFilter(const std::string&);

#pragma region INLINE_REGION
	inline void dg::DebugConsoleMessage(const char* message) {
#ifdef _DEBUG
		std::cout<<message;
#endif
	}

	inline glm::vec3 dg::DirectionFromRotation(const glm::vec3& direction, const glm::vec3& rotation) {
		return DirectionFromRotation(direction, EulerAngleToQuaternion(rotation));
	}

	inline glm::vec3 dg::DirectionFromRotation(const glm::vec3& direction, const glm::quat& rotation) {
		return glm::normalize(glm::rotate(rotation, direction));
	}

	inline glm::vec3 dg::ForwardDirectionFromRotation(const glm::vec3& rotation) {
		return DirectionFromRotation(vec_forward, rotation);
	}

	inline glm::vec3 dg::UpDirectionFromRotation(const glm::vec3& rotation) {
		return DirectionFromRotation(vec_up, rotation);
	}

	inline glm::vec3 dg::RightDirectionFromRotation(const glm::vec3& rotation) {
		return DirectionFromRotation(vec_right, rotation);
	}

	inline glm::vec3 dg::ForwardDirectionFromRotation(const glm::quat& rotation) {
		return DirectionFromRotation(vec_forward, rotation);
	}

	inline glm::vec3 dg::UpDirectionFromRotation(const glm::quat& rotation) {
		return DirectionFromRotation(vec_up, rotation);
	}

	inline glm::vec3 dg::RightDirectionFromRotation(const glm::quat& rotation) {
		return DirectionFromRotation(vec_right, rotation);
	}

	inline float dg::RandomFloat() {
		float Max = RAND_MAX;
		return ((float) RANDOM()/Max);
	}

	inline bool dg::CheckGLError() {
#ifdef _DEBUG
		GLenum error = glGetError();
		if(error!=GL_NO_ERROR) {
			dg::InfoGLError(error);
			return false;
		}
#endif
		return true;
	}

	inline eTextureType dg::ConvertToTextureType(const std::string& value) {
		if(value=="DIFFUSE_TEXTURE") {
			return eTextureType::DIFFUSE_TEXTURE;
		} else if(value=="SPECULAR_TEXTURE") {
			return eTextureType::SPECULAR_TEXTURE;
		} else if(value=="AMBIENT_TEXTURE") {
			return eTextureType::AMBIENT_TEXTURE;
		} else if(value=="EMISSIVE_TEXTURE") {
			return eTextureType::EMISSIVE_TEXTURE;
		} else if(value=="HEIGHT_TEXTURE") {
			return eTextureType::HEIGHT_TEXTURE;
		} else if(value=="NORMAL_TEXTURE") {
			return eTextureType::NORMAL_TEXTURE;
		} else if(value=="SHININESS_TEXTURE") {
			return eTextureType::SHININESS_TEXTURE;
		} else if(value=="OPACITY_TEXTURE") {
			return eTextureType::OPACITY_TEXTURE;
		} else if(value=="DISPLACEMENT_TEXTURE") {
			return eTextureType::DISPLACEMENT_TEXTURE;
		} else if(value=="LIGHTMAP_TEXTURE") {
			return eTextureType::LIGHTMAP_TEXTURE;
		} else if(value=="REFLECTION_TEXTURE") {
			return eTextureType::REFLECTION_TEXTURE;
		} else if(value=="UNKNOW_TEXTURE") {
			return eTextureType::UNKNOW_TEXTURE;
		} 

		return eTextureType::NONE_TEXTURE;
	}
	
	inline eTextureWrapParameter dg::ConvertToTextureWrap(const std::string& value) {
		if(value=="WRAP_REPEAT") {
			return eTextureWrapParameter::WRAP_REPEAT;
		} else if(value=="WRAP_CLAMP_TO_EDGE") {
			return eTextureWrapParameter::WRAP_CLAMP_TO_EDGE;
		} else if(value=="WRAP_CLAMP_TO_BORDER") {
			return eTextureWrapParameter::WRAP_CLAMP_TO_BORDER;
		} else if(value=="WRAP_MIRRORED_REPEAT") {
			return eTextureWrapParameter::WRAP_MIRRORED_REPEAT;
		}

		return eTextureWrapParameter::WRAP_REPEAT;
	}

	inline eTextureFilterParameter dg::ConvertToTextureFilter(const std::string& value) {
		if(value=="FILTER_NEAREST") {
			return eTextureFilterParameter::FILTER_NEAREST;
		} else if(value=="FILTER_LINEAR") {
			return eTextureFilterParameter::FILTER_LINEAR;
		} else if(value=="FILTER_NEAREST_MIPMAP_NEAREST") {
			return eTextureFilterParameter::FILTER_NEAREST_MIPMAP_NEAREST;
		} else if(value=="FILTER_LINEAR_MIPMAP_NEAREST") {
			return eTextureFilterParameter::FILTER_LINEAR_MIPMAP_NEAREST;
		} else if(value=="FILTER_NEAREST_MIPMAP_LINEAR") {
			return eTextureFilterParameter::FILTER_NEAREST_MIPMAP_LINEAR;
		} else if(value=="FILTER_LINEAR_MIPMAP_LINEAR") {
			return eTextureFilterParameter::FILTER_LINEAR_MIPMAP_LINEAR;
		}

		return eTextureFilterParameter::FILTER_NEAREST;
	}

#pragma endregion
}
