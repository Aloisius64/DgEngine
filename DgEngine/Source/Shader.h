////////////////////////////////////////////////////////////////////////////////
// Filename: Shader.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <map>
#include <string>
#include <vector>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "SceneObject.h"
#include "Common.h"
#include "Material.h"
#include "Mathematics.h"
#include "Glossary.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class Shader {
	public:
		Shader(string);
		Shader(string, string, string, string, string);
		Shader(const Shader&);

		virtual ~Shader();

		virtual bool Initialize();
		virtual void SetParameters(SceneObject*);
		virtual void BindMaterialParameters(const Material&);

		void SetActive();
		inline GLint GetUniformLocation(const char*);
		const string& GetName() const;
		const string& GetFileName(eShaderData) const;

		void SetInt(const char*, int);
		void SetFloat(const char*, float);
		void SetDouble(const char*, double);
		void SetIntVector2(const char*, ivec2);
		void SetFloatVector2(const char*, vec2);
		void SetDoubleVector2(const char*, dvec2);
		void SetIntVector3(const char*, ivec3);
		void SetFloatVector3(const char*, vec3);
		void SetDoubleVector3(const char*, dvec3);
		void SetIntVector4(const char*, ivec4);
		void SetFloatVector4(const char*, vec4);
		void SetDoubleVector4(const char*, dvec4);
		void SetFloatMatrix2(const char*, mat2);
		void SetFloatMatrix3(const char*, mat3);
		void SetFloatMatrix4(const char*, mat4);
		void SetDoubleMatrix4(const char*, dmat4);

		void SetInt(const char*, const vector<int>&);
		void SetFloat(const char*, const vector<float>&);
		void SetDouble(const char*, const vector<double>&);
		void SetIntVector2(const char*, const vector<ivec2>&);
		void SetFloatVector2(const char*, const vector<vec2>&);
		void SetDoubleVector2(const char*, const vector<dvec2>&);
		void SetIntVector3(const char*, const vector<ivec3>&);
		void SetFloatVector3(const char*, const vector<mat4>&);
		void SetDoubleVector3(const char*, const vector<dvec3>&);
		void SetIntVector4(const char*, const vector<ivec4>&);
		void SetFloatVector4(const char*, const vector<vec4>&);
		void SetDoubleVector4(const char*, const vector<dvec4>&);
		void SetFloatMatrix2(const char*, const vector<mat2>&);
		void SetFloatMatrix3(const char*, const vector<mat3>&);
		void SetFloatMatrix4(const char*, const vector<mat4>&);
		void SetDoubleMatrix4(const char*, const vector<dmat4>&);

	protected:
		GLint GetProgramParam(GLint);
		GLuint AddShader(GLenum, const char*);
		bool LinkProgram();

	protected:
		GLuint	m_ShaderProgramName;
		string	m_ShaderName;

	private:
		typedef map<string, GLuint, less<string>> LocationMap;
		GLuint		m_ShaderObject[eShaderData::NUM_SHADERs];
		string		m_ShaderFile[eShaderData::NUM_SHADERs];
		LocationMap m_Location;
	};
}
