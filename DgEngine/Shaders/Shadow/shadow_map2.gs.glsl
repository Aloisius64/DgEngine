#version 450 core

#define MAX_SHADOWS 20

layout(triangles) in;
layout(triangle_strip, max_vertices = 60) out;

uniform mat4 LightSpaceMatrix[MAX_SHADOWS];

out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main() {
	for(int face = 0; face<MAX_SHADOWS; ++face) {
		gl_Layer = face; // built-in variable that specifies to which face we render.

		for(int i = 0; i<3; ++i) // for each triangle's vertices
		{
			FragPos = gl_in[i].gl_Position;
			gl_Position = LightSpaceMatrix[face]*FragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}

