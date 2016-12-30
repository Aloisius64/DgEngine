#version 450 core

layout(location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
layout(location = 1) in vec4 color;

uniform mat4 projection;

struct VS_OUTPUT {
	vec2 TexCoords;
	vec4 Color;
};

out VS_OUTPUT vsOutput;

void main() {
	gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
	vsOutput.TexCoords = vertex.zw;
	vsOutput.Color = color;
}
