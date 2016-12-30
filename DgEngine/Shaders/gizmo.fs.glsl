#version 450 core

layout(early_fragment_tests) in;
layout(binding = 0, r32ui) uniform uimage2D head_pointer_image;
layout(binding = 1, rgba32ui) uniform writeonly uimageBuffer list_buffer;
layout(binding = 0, offset = 0) uniform atomic_uint list_counter;

struct VS_Output {
	vec2 TexCoord;
};

in VS_Output vsOutput;

layout(location = 0)out vec4 FragColor;
layout(location = 1) out vec4 BloomColor;

uniform bool AlbedoMap_bool;
uniform sampler2D AlbedoMap;
uniform vec4 Color;
// OIT.
uniform bool ActiveOIT;

void main() {
	FragColor = Color;

	if(AlbedoMap_bool) {
		FragColor = texture(AlbedoMap, vsOutput.TexCoord.xy);
	}

	if(ActiveOIT) {
		uint index;
		uint old_head;
		uvec4 item;
		vec4 frag_color;

		index = atomicCounterIncrement(list_counter);

		old_head = imageAtomicExchange(head_pointer_image, ivec2(gl_FragCoord.xy), uint(index));

		item.x = old_head;
		item.y = packUnorm4x8(FragColor);
		item.z = floatBitsToUint(gl_FragCoord.z);
		item.w = 0; // packUnorm4x8(additive_component);

		imageStore(list_buffer, int(index), item);
	}
	
	// Assign color to bloom buffer.
	BloomColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
}
