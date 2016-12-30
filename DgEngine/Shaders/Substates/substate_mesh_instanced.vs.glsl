#version 450 core

layout(location = 0)	in vec3		Position;
layout(location = 1)	in vec2		TexCoord;
layout(location = 2)	in vec3		Normal;
layout(location = 3)	in vec3		Tangent;
layout(location = 4)	in mat4		I_ModelMatrix;
layout(location = 8)	in int		I_SubstateDataByte;
layout(location = 9)	in int		I_SubstateDataInt;
layout(location = 10)	in double	I_SubstateDataReal;

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

flat out VS_Output vsOutput;

void main() {
	gl_Position = vec4(Position, 1.0);
	vsOutput.Position = Position;
	vsOutput.TexCoord = TexCoord;
	vsOutput.Normal = Normal;
	vsOutput.Tangent = Tangent;
	vsOutput.ModelMatrix = I_ModelMatrix;
	vsOutput.SubstateDataByte = I_SubstateDataByte;
	vsOutput.SubstateDataInt = I_SubstateDataInt;
	vsOutput.SubstateDataReal = I_SubstateDataReal;
}
