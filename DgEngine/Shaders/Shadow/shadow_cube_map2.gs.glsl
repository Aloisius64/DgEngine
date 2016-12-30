#version 450 core

#define NUM_FACE 6

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform int shadow;
uniform mat4 shadowTransforms[NUM_FACE];

out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main() {
	//for(int shadow = 0; shadow<MAX_SHADOWS; ++shadow) {
		for(int face = 0; face<NUM_FACE; ++face) {
			gl_Layer = face+(shadow*NUM_FACE);
			for(int i = 0; i<3; ++i) {
				FragPos = gl_in[i].gl_Position;
				gl_Position = shadowTransforms[face]*FragPos;
				EmitVertex();
			}
			EndPrimitive();
		}
	//}
}

