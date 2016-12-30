////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderSSAO.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Shader.h"
#include "Environment.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class ShaderSSAO : public Shader {
	public:
		ShaderSSAO()
			: Shader("./Shaders/SSAO/ssao.vs.glsl",
			"", "", "",
			"./Shaders/SSAO/ssao.fs.glsl") {
			m_ShaderName = SHADER_SSAO;
		}

		virtual ~ShaderSSAO() {}

		virtual bool Initialize() {
			if(!Shader::Initialize()) {
				return false;
			}

			// Initialize kernel.
			SetActive();
			unsigned int kernelSize = Environment::Instance()->GetKernelSize();
			vec3* kernel = new vec3[kernelSize];
			for(unsigned int i = 0; i<kernelSize; i++) {
				float scale = (float) i/(float) (kernelSize);
				vec3 v;
				v[0] = 2.0f * (float) rand()/RAND_MAX-1.0f;
				v[1] = 2.0f * (float) rand()/RAND_MAX-1.0f;
				v[2] = 2.0f * (float) rand()/RAND_MAX-1.0f;
				// Use an acceleration function so more points are
				// located closer to the origin
				v *= (0.1f+0.9f * scale * scale);

				kernel[i] = v;
			}
			glUniform3fv(GetUniformLocation("gKernel"), kernelSize, glm::value_ptr(kernel[0]));
			delete kernel;
			SetFloat("gSampleRad", 3.0f);
			SetInt("gPositionMap", POSITION_TEXTURE_UNIT_INDEX);

			return true;
		}
	};
}
