#version 450 core

layout(location = 0) in vec3 Position;
//layout(location = 8) in mat4 I_ModelMatrix;

uniform mat4 MMatrix;
//uniform bool Instancing;			// Instanced rendering active

void main() {
	gl_Position = MMatrix * vec4(Position, 1.0);
	//if(Instancing) {
	//gl_Position = I_ModelMatrix * vec4(Position, 1.0);
	//}
}
