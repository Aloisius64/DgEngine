#version 450 core

struct VS_OUTPUT {
	vec2 TexCoords;
	vec4 Color;
};

in VS_OUTPUT vsOutput;

out vec4 FragColor;

void main() {
	// Checkboard pattern.
	if(vsOutput.Color.a<1.0f) {
		vec2 fw = fwidth(vsOutput.TexCoords);
		float frequency = 2.5f;
		vec2 fuzz = fw * frequency;
		float fuzzMax = max(fuzz.s, fuzz.t);
		vec2 checkPos = fract(vsOutput.TexCoords * frequency);

		vec4 colorBlack = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		vec4 colorWhite = vec4(1.0f, 1.0f, 1.0f, 1.0f);

		vec2 p = smoothstep(vec2(0.5), fuzz+vec2(0.5), checkPos)+
			(1.0-smoothstep(vec2(0.0), fuzz, checkPos));

		vec4 patternColor = mix(colorBlack, colorWhite, p.x*p.y+(1.0-p.x)*(1.0-p.y));
		
		FragColor = mix(patternColor, vsOutput.Color, vsOutput.Color.a);
	} else {
		FragColor = vsOutput.Color;
	}
}
