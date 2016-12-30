#version 450 core

uniform sampler2D imageLogo;

in vec2 texCoord;

layout(location = 0) out vec4 FragColor;

void main() {
	vec2 tc = texCoord;
	tc.t = 1.0f-tc.t;
	FragColor = texture(imageLogo, tc);
}
