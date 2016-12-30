#version 450 core

layout(location = 0) in vec4 Position;

layout(location = 0) uniform mat4 MVPMatrix;
layout(location = 1) uniform mat4 MMatrix;

out vec4 vRayHit;
out vec3 EntryPoint;

void main() {
	EntryPoint = Position.xyz+vec3(0.5f);
	vRayHit = MMatrix * Position;
	gl_Position = MVPMatrix * Position;
}
