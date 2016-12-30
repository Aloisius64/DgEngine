#version 450 core

#define MAX_SHADOW 20
#define CODE_SUBSTATE_BYTE	0
#define CODE_SUBSTATE_INT	1
#define CODE_SUBSTATE_REAL	2

layout(triangles) in;
layout(triangle_strip, max_vertices = 6) out;

uniform mat4	VPMatrix;
uniform mat4	MMatrix;
uniform int		NumShadow;
uniform mat4	LightSpaceMatrix[MAX_SHADOW];
uniform int		SubstateType;
uniform int		NoDataByte;
uniform int		NoDataInt;
uniform double	NoDataReal;
uniform int		MinValueByte;
uniform int		MaxValueByte;
uniform int		MinValueInt;
uniform int		MaxValueInt;
uniform double	MinValueReal;
uniform double	MaxValueReal;


struct VS_Output {
	vec3 Position;
	vec2 TexCoord;
	vec3 Normal;
	vec3 Tangent;
	mat4 ModelMatrix;
	int SubstateDataByte;
	int SubstateDataInt;
	double SubstateDataReal;
};

struct GS_Output {
	vec3 Position;
	vec2 TexCoord;
	vec3 Normal;
	vec3 Tangent;
	vec4 LightSpacePosition[MAX_SHADOW];
	float SubstateColorRatio;
};

flat in VS_Output vsOutput[];
out GS_Output gsOutput;

void main() {
	bool draw = true;
	for(int i = 0; i<gl_in.length(); ++i) {
		if(SubstateType==CODE_SUBSTATE_BYTE) {
			draw = draw&&(vsOutput[i].SubstateDataByte!=NoDataByte);
		} else if(SubstateType==CODE_SUBSTATE_INT) {
			draw = draw&&(vsOutput[i].SubstateDataInt!=NoDataInt);
		} else if(SubstateType==CODE_SUBSTATE_REAL) {
			draw = draw&&(vsOutput[i].SubstateDataReal!=NoDataReal);
		}
	}

	if(draw) {

		mat3 NormalMatrix = mat3(transpose(inverse(MMatrix * vsOutput[0].ModelMatrix)));
		mat4 ModelMatrix = MMatrix * vsOutput[0].ModelMatrix;
		mat4 MVPMatrix = VPMatrix * ModelMatrix;
		
		// Computing color ratio.
		if(SubstateType==CODE_SUBSTATE_BYTE) {
			gsOutput.SubstateColorRatio = (vsOutput[0].SubstateDataByte-MinValueByte)/(MaxValueByte-MinValueByte);
		} else if(SubstateType==CODE_SUBSTATE_INT) {
			gsOutput.SubstateColorRatio = (vsOutput[0].SubstateDataInt-MinValueInt)/(MaxValueInt-MinValueInt);
		} else if(SubstateType==CODE_SUBSTATE_REAL) {
			gsOutput.SubstateColorRatio = float(vsOutput[0].SubstateDataReal-MinValueReal)/float(MaxValueReal-MinValueReal);
		}

		for(int i = 0; i<gl_in.length(); ++i) {
			gl_Position = MVPMatrix * vec4(vsOutput[i].Position, 1.0);
			gsOutput.Position = (ModelMatrix * vec4(vsOutput[i].Position, 1.0)).xyz;
			gsOutput.TexCoord = vsOutput[i].TexCoord;
			gsOutput.Normal = NormalMatrix * vsOutput[i].Normal;
			gsOutput.Tangent = (vsOutput[i].ModelMatrix * vec4(vsOutput[i].Tangent, 0.0)).xyz;

			// Shadows
			for(int i = 0; i<MAX_SHADOW && i<NumShadow; i++) {
				gsOutput.LightSpacePosition[i] = LightSpaceMatrix[i]*vec4(vsOutput[i].Position, 1.0);
			}

			EmitVertex();
		}
		EndPrimitive();
	}
}
