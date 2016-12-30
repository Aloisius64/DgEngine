#version 450 core

layout(location = 0) in vec3 Position;

struct VS_Output {
	vec3 TexCoord;
};

out VS_Output vsOutput;

uniform mat4 MVPMatrix;                                                         

void main() {
	vsOutput.TexCoord = Position;                                                 
	gl_Position = (MVPMatrix * vec4(Position, 1.0)).xyww;
}
