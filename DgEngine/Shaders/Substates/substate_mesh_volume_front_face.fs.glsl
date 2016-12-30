#version 450 core

layout(location = 4) uniform vec3 eyePos;

in vec4 vRayHit;
in vec3 EntryPoint;

layout(location = 0, index = 0) out float Distance;


void main() {
	Distance = length(eyePos-vRayHit.xyz);
}
