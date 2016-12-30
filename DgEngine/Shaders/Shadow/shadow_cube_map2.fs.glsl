#version 450 core

in vec4 FragPos;

uniform float far_plane;
uniform vec3 lightPos;

void main() {
	float lightDistance = length(FragPos.xyz-lightPos);

	// map to [0;1] range by dividing by far_plane
	lightDistance = lightDistance/far_plane;

	// Write this as modified depth
	gl_FragDepth = lightDistance;
}
