#version 450 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 Tangent;
layout(location = 4) in mat4 I_MVPMatrix;
layout(location = 8) in mat4 I_ModelMatrix;
layout(location = 12) in ivec4 BoneIDs;
layout(location = 13) in vec4 Weights;

const int MAX_BONES = 100;
#define MAX_SHADOW 20

uniform mat4 MVPMatrix;
uniform mat4 MMatrix;
uniform mat3 NormalMatrix;
uniform bool Instancing;			// Instanced rendering active
uniform int NumShadow;				// Num shadows
uniform mat4 LightSpaceMatrix[MAX_SHADOW];	
uniform mat4 Bones[MAX_BONES];

struct VS_Output {
	vec3 Position;
	vec2 TexCoord;
	vec3 Normal;
	vec3 Tangent;
	vec4 LightSpacePosition[MAX_SHADOW]; // Shadows
};

out VS_Output vsOutput;

void main() {
	mat4 BoneTransform = Bones[BoneIDs[0]]*Weights[0];
	BoneTransform += Bones[BoneIDs[1]]*Weights[1];
	BoneTransform += Bones[BoneIDs[2]]*Weights[2];
	BoneTransform += Bones[BoneIDs[3]]*Weights[3];

	if(!Instancing) {
		gl_Position = MVPMatrix * BoneTransform * vec4(Position, 1.0);
		vsOutput.Position = (MMatrix * BoneTransform * vec4(Position, 1.0)).xyz;
		vsOutput.TexCoord = TexCoord;
		vsOutput.Normal = NormalMatrix * Normal;
		vsOutput.Tangent = (MMatrix * vec4(Tangent, 0.0)).xyz;
	} else {
		gl_Position = I_MVPMatrix * BoneTransform * vec4(Position, 1.0);
		vsOutput.Position = (I_ModelMatrix * BoneTransform * vec4(Position, 1.0)).xyz;
		vsOutput.TexCoord = TexCoord;
		// Add Normal Matrix also in instanced rendering.
		vsOutput.Normal = mat3(transpose(inverse(I_ModelMatrix * BoneTransform))) * Normal;
		vsOutput.Tangent = (I_ModelMatrix * vec4(Tangent, 0.0)).xyz;
	}

	// Shadows
	for(int i = 0; i<MAX_SHADOW && i<NumShadow; i++) {
		vsOutput.LightSpacePosition[i] = LightSpaceMatrix[i]*BoneTransform*vec4(vsOutput.Position, 1.0);
	}
}
