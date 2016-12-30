#version 450 core

layout(location = 0) in vec3 Position;

uniform mat4 LightSpaceMatrix;
uniform mat4 ModelMatrix;

void main() {
	gl_Position = LightSpaceMatrix * ModelMatrix * vec4(Position, 1.0f);
}
