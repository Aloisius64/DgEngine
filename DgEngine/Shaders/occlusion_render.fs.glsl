#version 450 core

// Out variables.
layout(location = 0) out vec4 FragColor;
//layout(location = 1) out vec4 BloomColor;

void main() {
	FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//BloomColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
