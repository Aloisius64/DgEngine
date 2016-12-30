#version 450 core

layout(location = 0) in vec3 Position;
layout(location = 2) in vec3 Normal;

out VS_OUT{
	vec3 normal;
} vs_out;

uniform mat4 PMatrix;
uniform mat4 MVPMatrix;
uniform mat3 NormalMatrix;

void main() {
	gl_Position = MVPMatrix * vec4(Position, 1.0f);
	vs_out.normal = normalize(vec3(PMatrix * vec4(NormalMatrix * Normal, 1.0f)));
}
