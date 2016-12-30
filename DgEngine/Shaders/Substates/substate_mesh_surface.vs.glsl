#version 450 core

#define MAX_SHADOW 20
#define CODE_SUBSTATE_BYTE	0
#define CODE_SUBSTATE_INT	1
#define CODE_SUBSTATE_REAL	2

layout(location = 0)	in vec3		Position;
layout(location = 1)	in vec2		TexCoord;
layout(location = 2)	in vec3		Normal;
layout(location = 3)	in vec3		Tangent;
layout(location = 4)	in mat4		I_ModelMatrix;
layout(location = 8)	in int		SubstateDataByte;
layout(location = 9)	in int		SubstateDataInt;
layout(location = 10)	in double	SubstateDataReal;

uniform mat4	MVPMatrix;
uniform mat4	MMatrix;
uniform mat3	NormalMatrix;
uniform int		NumShadow;				// Num shadows
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
uniform vec3	PreTranslation;
uniform vec3	PreScaling;

struct VS_Output {
	vec3 Position;
	vec2 TexCoord;
	vec3 Normal;
	vec3 Tangent;
	vec4 LightSpacePosition[MAX_SHADOW];
	float SubstateColorRatio;
};

out VS_Output vsOutput;

void main() {
	vec3 TransformedPosition = (Position-PreTranslation)*PreScaling; // Center the model to origin.

	gl_Position = MVPMatrix * vec4(TransformedPosition, 1.0);
	vsOutput.Position = (MMatrix * vec4(TransformedPosition, 1.0)).xyz;
	vsOutput.TexCoord = TexCoord;
	vsOutput.Normal = NormalMatrix * Normal;
	vsOutput.Tangent = (MMatrix * vec4(Tangent, 0.0)).xyz;
	
	// Computing color ratio.
	if(SubstateType==CODE_SUBSTATE_BYTE) {
		vsOutput.SubstateColorRatio = (SubstateDataByte-MinValueByte)/(MaxValueByte-MinValueByte);
	} else if(SubstateType==CODE_SUBSTATE_INT) {
		vsOutput.SubstateColorRatio = (SubstateDataInt-MinValueInt)/(MaxValueInt-MinValueInt);
	} else if(SubstateType==CODE_SUBSTATE_REAL) {
		vsOutput.SubstateColorRatio = float(SubstateDataReal-MinValueReal)/float(MaxValueReal-MinValueReal);
	}

	// Shadows
	for(int i = 0; i<MAX_SHADOW && i<NumShadow; i++) {
		vsOutput.LightSpacePosition[i] = LightSpaceMatrix[i]*vec4(vsOutput.Position, 1.0);
	}
}
