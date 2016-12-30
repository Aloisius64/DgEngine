#version 450 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoord;

uniform mat4 MVPMatrix;

struct VS_Output {
	vec2 TexCoord;
};

out VS_Output vsOutput;

void main() {
	vsOutput.TexCoord = TexCoord;
	gl_Position = MVPMatrix * vec4(Position, 1.0);
}