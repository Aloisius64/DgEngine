#version 450 core

layout(location = 0) in vec3 Position;
layout(location = 4) in mat4 I_MVPMatrix;
layout(location = 8) in mat4 I_ModelMatrix;

uniform mat4 MVPMatrix;
uniform bool Instancing;

void main() {
	if(!Instancing) {
		gl_Position = MVPMatrix * vec4(Position, 1.0);
	} else {
		gl_Position = I_MVPMatrix * vec4(Position, 1.0);
	}
}
