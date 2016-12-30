////////////////////////////////////////////////////////////////////////////////
// Filename: Shader.cpp
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Shader.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;
using namespace dg;

static int Shaders[] = {
	GL_VERTEX_SHADER,
	GL_TESS_CONTROL_SHADER,
	GL_TESS_EVALUATION_SHADER,
	GL_GEOMETRY_SHADER,
	GL_FRAGMENT_SHADER,
	GL_COMPUTE_SHADER
};

Shader::Shader(string computeShader) {
	m_ShaderProgramName = 0;
	m_ShaderName = "";

	m_ShaderFile[eShaderData::COMPUTE_SHADER] = computeShader;
}

Shader::Shader(string vertexShader,
	string tessellationControlShader,
	string tessellationEvaluationShader,
	string geometryShader,
	string fragmentShader) {

	m_ShaderProgramName = 0;
	m_ShaderName = "";

	m_ShaderFile[eShaderData::VERTEX_SHADER] = vertexShader;
	m_ShaderFile[eShaderData::TESSELLATION_CONTROL_SHADER] = tessellationControlShader;
	m_ShaderFile[eShaderData::TESSELLATION_EVALUATION_SHADER] = tessellationEvaluationShader;
	m_ShaderFile[eShaderData::GEOMETRY_SHADER] = geometryShader;
	m_ShaderFile[eShaderData::FRAGMENT_SHADER] = fragmentShader;

	m_ShaderObject[eShaderData::VERTEX_SHADER] = 0;
	m_ShaderObject[eShaderData::TESSELLATION_CONTROL_SHADER] = 0;
	m_ShaderObject[eShaderData::TESSELLATION_EVALUATION_SHADER] = 0;
	m_ShaderObject[eShaderData::GEOMETRY_SHADER] = 0;
	m_ShaderObject[eShaderData::FRAGMENT_SHADER] = 0;
}

Shader::Shader(const Shader& shader) {
	m_ShaderProgramName = shader.m_ShaderProgramName;
	m_ShaderName = shader.m_ShaderName;
	for(size_t i = 0; i<eShaderData::NUM_SHADERs; i++) {
		m_ShaderObject[i] = shader.m_ShaderObject[i];
		m_ShaderFile[i] = shader.m_ShaderFile[i];
	}
	for each (auto pair in shader.m_Location) {
		m_Location[pair.first] = pair.second;
	}
}

Shader::~Shader() {
	m_Location.clear();

	for(size_t i = 0; i<eShaderData::NUM_SHADERs; i++) {
		if(glIsShader(m_ShaderObject[i]))
			glDeleteShader(m_ShaderObject[i]);
	}

	if(glIsProgram(m_ShaderProgramName)) {
		glDeleteProgram(m_ShaderProgramName);
		m_ShaderProgramName = 0;
	}
}

bool Shader::Initialize() {
	m_ShaderProgramName = glCreateProgram();

	if(m_ShaderProgramName==0) {
		fprintf(stderr, "Error creating shader program\n");
		return false;
	}

	for(size_t i = 0; i<eShaderData::NUM_SHADERs; i++) {
		if(m_ShaderFile[i]!="") {
			m_ShaderObject[i] = AddShader(Shaders[i], m_ShaderFile[i].c_str());
			if(m_ShaderObject[i]==0) {
				return false;
			}
		}
	}

	return LinkProgram();
}

void Shader::SetParameters(SceneObject* sceneObject) {
	SetActive();

	dg::CheckGLError();
}

void Shader::BindMaterialParameters(const Material& material) {

}

const string& Shader::GetName() const {
	return m_ShaderName;
}

const string& Shader::GetFileName(eShaderData shaderData) const {
	return m_ShaderFile[shaderData];
}

void Shader::SetActive() {
	glUseProgram(m_ShaderProgramName);
}

GLuint Shader::AddShader(GLenum shaderType, const char* filename) {
	string s;

	if(!dg::ReadFile(filename, s)) {
		return false;
	}

	GLuint ShaderObj = glCreateShader(shaderType);

	if(ShaderObj==0) {
		fprintf(stderr, "Error creating shader type %d\n", shaderType);
		return false;
	}

	const GLchar* p[1];
	p[0] = s.c_str();
	GLint Lengths[1] = {(GLint) s.size()};

	glShaderSource(ShaderObj, 1, p, Lengths);

	glCompileShader(ShaderObj);

	GLint success;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

	if(!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling '%s': '%s'\n", filename, InfoLog);
		return false;
	}

	glAttachShader(m_ShaderProgramName, ShaderObj);

	return ShaderObj;
}

bool Shader::LinkProgram() {
	GLint Success = 0;
	GLchar ErrorLog[1024] = {0};

	glLinkProgram(m_ShaderProgramName);

	glGetProgramiv(m_ShaderProgramName, GL_LINK_STATUS, &Success);
	if(Success==0) {
		glGetProgramInfoLog(m_ShaderProgramName, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		return false;
	}

	glValidateProgram(m_ShaderProgramName);
	glGetProgramiv(m_ShaderProgramName, GL_VALIDATE_STATUS, &Success);
	if(!Success) {
		glGetProgramInfoLog(m_ShaderProgramName, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		return false;
	}

	return dg::CheckGLError();
}

GLint Shader::GetUniformLocation(const char* uniformName) {
	string uniformNameString = uniformName;
	if(!m_Location[uniformNameString]) {
		GLuint uniformLocation = glGetUniformLocation(m_ShaderProgramName, uniformName);
		if(uniformLocation==INVALID_UNIFORM_LOCATION) {
			dg::DebugConsoleMessage("Warning! Unable to get the location of uniform: ");
			dg::DebugConsoleMessage(uniformName);
			dg::DebugConsoleMessage("\n");
		}
		m_Location[uniformNameString] = uniformLocation;
	}
	return m_Location[uniformNameString];
}

GLint Shader::GetProgramParam(GLint param) {
	GLint ret;
	glGetProgramiv(m_ShaderProgramName, param, &ret);
	return ret;
}

void Shader::SetInt(const char* uniformName, int value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform1i(location, value);
	}
}

void Shader::SetFloat(const char* uniformName, float value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform1f(location, value);
	}
}

void Shader::SetDouble(const char* uniformName, double value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform1d(location, value);
	}
}

void Shader::SetIntVector2(const char* uniformName, ivec2 value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform2iv(location, 1, glm::value_ptr(value));
	}
}

void Shader::SetFloatVector2(const char* uniformName, vec2 value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform2fv(location, 1, glm::value_ptr(value));
	}
}

void Shader::SetDoubleVector2(const char* uniformName, dvec2 value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform2dv(location, 1, glm::value_ptr(value));
	}
}

void Shader::SetIntVector3(const char* uniformName, ivec3 value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform3iv(location, 1, glm::value_ptr(value));
	}
}

void Shader::SetFloatVector3(const char* uniformName, vec3 value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform3fv(location, 1, glm::value_ptr(value));
	}
}

void Shader::SetDoubleVector3(const char* uniformName, dvec3 value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform3dv(location, 1, glm::value_ptr(value));
	}
}

void Shader::SetIntVector4(const char* uniformName, ivec4 value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform4iv(location, 1, glm::value_ptr(value));
	}
}

void Shader::SetFloatVector4(const char* uniformName, vec4 value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform4fv(location, 1, glm::value_ptr(value));
	}
}

void Shader::SetDoubleVector4(const char* uniformName, dvec4 value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform4dv(location, 1, glm::value_ptr(value));
	}
}

void Shader::SetFloatMatrix2(const char* uniformName, mat2 value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
}

void Shader::SetFloatMatrix3(const char* uniformName, mat3 value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
}

void Shader::SetFloatMatrix4(const char* uniformName, mat4 value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
}

void Shader::SetDoubleMatrix4(const char* uniformName, dmat4 value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniformMatrix4dv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
}

void Shader::SetInt(const char* uniformName, const vector<int>& value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform1iv(location, value.size(), &value.front());
	}
}

void Shader::SetFloat(const char* uniformName, const vector<float>& value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform1fv(location, value.size(), &value.front());
	}
}

void Shader::SetDouble(const char* uniformName, const vector<double>& value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform1dv(location, value.size(), &value.front());
	}
}

void Shader::SetIntVector2(const char* uniformName, const vector<ivec2>& value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform2iv(location, value.size(), glm::value_ptr(value.front()));
	}
}

void Shader::SetFloatVector2(const char* uniformName, const vector<vec2>& value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform2fv(location, value.size(), glm::value_ptr(value.front()));
	}
}

void Shader::SetDoubleVector2(const char* uniformName, const vector<dvec2>& value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform2dv(location, value.size(), glm::value_ptr(value.front()));
	}
}

void Shader::SetIntVector3(const char* uniformName, const vector<ivec3>& value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform3iv(location, value.size(), glm::value_ptr(value.front()));
	}
}

void Shader::SetFloatVector3(const char* uniformName, const vector<mat4>& value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform3fv(location, value.size(), glm::value_ptr(value.front()));
	}
}

void Shader::SetDoubleVector3(const char* uniformName, const vector<dvec3>& value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform3dv(location, value.size(), glm::value_ptr(value.front()));
	}
}

void Shader::SetIntVector4(const char* uniformName, const vector<ivec4>& value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform4iv(location, value.size(), glm::value_ptr(value.front()));
	}
}

void Shader::SetFloatVector4(const char* uniformName, const vector<vec4>& value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform4fv(location, value.size(), glm::value_ptr(value.front()));
	}
}

void Shader::SetDoubleVector4(const char* uniformName, const vector<dvec4>& value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniform4dv(location, value.size(), glm::value_ptr(value.front()));
	}
}

void Shader::SetFloatMatrix2(const char* uniformName, const vector<mat2>& value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniformMatrix2fv(location, value.size(), GL_FALSE, glm::value_ptr(value.front()));
	}
}

void Shader::SetFloatMatrix3(const char* uniformName, const vector<mat3>& value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniformMatrix3fv(location, value.size(), GL_FALSE, glm::value_ptr(value.front()));
	}
}

void Shader::SetFloatMatrix4(const char* uniformName, const vector<mat4>& value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniformMatrix4fv(location, value.size(), GL_FALSE, glm::value_ptr(value.front()));
	}
}

void Shader::SetDoubleMatrix4(const char* uniformName, const vector<dmat4>& value) {
	GLuint location = GetUniformLocation(uniformName);
	if(location!=INVALID_UNIFORM_LOCATION) {
		glUniformMatrix4dv(location, value.size(), GL_FALSE, glm::value_ptr(value.front()));
	}
}
