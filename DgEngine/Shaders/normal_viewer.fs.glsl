#version 450 core

//layout(early_fragment_tests) in;
//layout(binding = 0, r32ui) uniform uimage2D head_pointer_image;
//layout(binding = 1, rgba32ui) uniform writeonly uimageBuffer list_buffer;
//layout(binding = 0, offset = 0) uniform atomic_uint list_counter;

layout(location = 0) out vec4 FragColor;

//// Gamma.
//uniform float Gamma;
//// HDR.
//uniform bool ActiveHDR;
// OIT.
//uniform bool ActiveOIT;

void main() {

	FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
	
	//// Gamma correction if HDR and OIT are disabled.
	//if(!ActiveHDR) {
	//	FragColor.rgb = pow(FragColor.rgb, vec3(1.0/Gamma));
	//}

	//if(ActiveOIT) {
	//	uint index;
	//	uint old_head;
	//	uvec4 item;
	//	vec4 frag_color;

	//	index = atomicCounterIncrement(list_counter);

	//	old_head = imageAtomicExchange(head_pointer_image, ivec2(gl_FragCoord.xy), uint(index));

	//	item.x = old_head;
	//	item.y = packUnorm4x8(FragColor);
	//	item.z = floatBitsToUint(gl_FragCoord.z);
	//	item.w = 0; // packUnorm4x8(additive_component);

	//	imageStore(list_buffer, int(index), item);
	//}
}
