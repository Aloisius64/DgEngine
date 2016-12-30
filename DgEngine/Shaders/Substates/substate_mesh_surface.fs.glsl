#version 450 core

layout(early_fragment_tests) in;
layout(binding = 0, r32ui) uniform uimage2D head_pointer_image;
layout(binding = 1, rgba32ui) uniform writeonly uimageBuffer list_buffer;
layout(binding = 0, offset = 0) uniform atomic_uint list_counter;

// Lights.
#define MAX_LIGHTS			20
#define MAX_SHADOW			20
#define DIRECTIONAL_SHADOW	0
#define POINT_SHADOW		1
#define SPOT_SHADOW			2
#define NO_SHADOW			0
#define HARD_SHADOW			1
#define SOFT_SHADOW			2
// Fog.
#define FOG_LINEAR				0
#define FOG_EXPONENTIAL			1
#define FOG_EXPONENTIAL_SQUARED 2
#define FOG_ALTITUDE			3

struct Light {
	vec4 color;
	float ambient_intensity;
	float diffuse_intensity;
	float specular_strength;
	float constant_attenuation;
	float linear_attenuation;
	float exponential_attenuation;
	vec3 position;
	vec3 direction;
	float cutoff;
	bool isLocal;
	bool isSpot;
	int	shadowType;
};

// Material.
struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 emissive;
	float shininess;	// extra factor to adjust shininess
	float reflection;
	float refraction;
	float bump_scaling;
	float opacity;
	vec2 tc_scale;
	vec2 tc_offset;
};

struct VS_Output {
	vec3 Position;
	vec2 TexCoord;
	vec3 Normal;
	vec3 Tangent;
	vec4 LightSpacePosition[MAX_SHADOW];
	float SubstateColorRatio;
};

// Lights.
uniform vec3 EyePosition;	// camera position
uniform int NumLights;
uniform Light Lights[MAX_LIGHTS];
// Material.
uniform Material material;
uniform bool Shadeless;
// Textures.
uniform bool useAlbedoMap;
uniform sampler2D AlbedoMap;
uniform bool useNormalMap;
uniform sampler2D NormalMap;
uniform bool useSpecularMap;
uniform sampler2D SpecularMap;
uniform bool useShininessMap;
uniform sampler2D ShininessMap;
uniform bool useAmbientMap;
uniform sampler2D AmbientMap;
uniform bool useEmissiveMap;
uniform sampler2D EmissiveMap;
uniform bool useOpacityMap;
uniform sampler2D OpacityMap;
uniform bool useAOMap;
uniform sampler2D AOMap;
// Skybox.
uniform bool useSkybox;
uniform samplerCube Skybox;
// Shadows.
uniform bool shadowEnabled;
uniform int NumShadow;							// Num shadows
uniform int LightIndex[MAX_SHADOW];
uniform int ShadowType[MAX_SHADOW];				// 0 for Directional, 1 for Point and 2 for Spot 
uniform sampler2DArray ShadowMapArray;
uniform samplerCubeArray ShadowCubeMapArray;

uniform float FarPlane;
// AO.
uniform vec2 ScreenSize;
// Fog.
uniform bool FogActive;
uniform int FogType;
uniform float FogDensity;
uniform vec4 FogColor;
// Gamma.
uniform float Gamma;
// HDR.
uniform bool ActiveHDR;
// OIT.
uniform bool ActiveOIT;
// SubstateColors
uniform vec4 MinColor;
uniform vec4 MaxColor;

// Array of offset direction for sampling.
vec3 gridSamplingDisk[20] = vec3[](
	vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
	vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
	vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
	vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
	vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1));

// In variables.
in VS_Output vsOutput;
in vec4 LightSpacePosition[MAX_SHADOW];

// Out variables.
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BloomColor;


vec3 computeBumpedNormal() {
	if(!useNormalMap)
		return vsOutput.Normal;
	vec3 Normal = normalize(vsOutput.Normal);
	vec3 Tangent = normalize(vsOutput.Tangent);
	Tangent = normalize(Tangent-dot(Tangent, Normal) * Normal);
	vec3 Bitangent = cross(Tangent, Normal);
	vec3 BumpMapNormal = material.bump_scaling * texture(NormalMap, vsOutput.TexCoord).xyz;
	BumpMapNormal = 2.0 * BumpMapNormal-vec3(1.0, 1.0, 1.0);
	vec3 NewNormal;
	mat3 TBN = mat3(Tangent, Bitangent, Normal);
	NewNormal = TBN * BumpMapNormal;
	return normalize(NewNormal);
}

vec2 computeScreenTexCoord() {
	return gl_FragCoord.xy/ScreenSize;
}

float computeShadowMap(int i, int shadowIndex) {
	if(i>=NumShadow)
		return 1.0f;

	// perform perspective divide
	vec3 projCoords = vsOutput.LightSpacePosition[LightIndex[i]].xyz/vsOutput.LightSpacePosition[LightIndex[i]].w;
	// Transform to [0,1] range
	projCoords = projCoords * 0.5f+0.5f;
	if(projCoords.z<=1.0) {
		// Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
		float closestDepth = texture(ShadowMapArray, vec3(projCoords.xy, shadowIndex)).r;
		// Get depth of current fragment from light's perspective
		float currentDepth = projCoords.z;
		// Calculate bias (based on depth map resolution and slope)
		vec3 normal = normalize(vsOutput.Normal);
		vec3 lightDir = normalize(Lights[LightIndex[i]].position-vsOutput.Position);
		float bias = max(0.05 * (1.0-dot(normal, lightDir)), 0.005);
		float shadow = currentDepth-bias>closestDepth ? 1.0 : 0.0;

		if(Lights[LightIndex[i]].shadowType==SOFT_SHADOW) {	// Soft directional shadows
			vec3 texelSize = 1.0/textureSize(ShadowMapArray, 0);
			int shadowSamples = 3;

			for(int x = -shadowSamples; x<=shadowSamples; ++x) {
				for(int y = -shadowSamples; y<=shadowSamples; ++y) {
					float pcfDepth = texture(ShadowMapArray, vec3(projCoords.xy+vec2(x, y) * texelSize.xy, shadowIndex)).r;
					shadow += currentDepth-bias>pcfDepth ? 1.0 : 0.0;
				}
			}
			shadow /= (shadowSamples*2)*(shadowSamples*2);
		}

		return (1.0f-shadow);
	}

	return 0.0f;
}

float computeShadowCubeMap(int i, int shadowIndex) {
	if(i>=NumShadow)
		return 1.0f;

	vec3 fragToLight = vsOutput.Position-Lights[LightIndex[i]].position;
	float currentDepth = length(fragToLight);
	float bias = 0.05f;
	float shadow = 0.0f;
	if(Lights[LightIndex[i]].shadowType==HARD_SHADOW) {			// Hard point shadows
		//float closestDepth = texture(ShadowCubeMap, fragToLight).r;
		float closestDepth = texture(ShadowCubeMapArray, vec4(fragToLight, shadowIndex)).r;
		closestDepth *= FarPlane;
		shadow = currentDepth-bias>closestDepth ? 1.0 : 0.0;
		shadow = (1.0f-shadow);
	} else if(Lights[LightIndex[i]].shadowType==SOFT_SHADOW) {	// Soft point shadows
		int samples = 20;
		float viewDistance = length(EyePosition-vsOutput.Position);
		float diskRadius = (1.0+(viewDistance/FarPlane))/25.0f;
		for(int j = 0; j<samples; ++j) {
			//float closestDepth = texture(ShadowCubeMap, fragToLight+gridSamplingDisk[j]*diskRadius).r;
			float closestDepth = texture(ShadowCubeMapArray, vec4(fragToLight+gridSamplingDisk[j]*diskRadius, shadowIndex)).r;
			// Undo mapping [0;1].
			closestDepth *= FarPlane;
			if(currentDepth-bias>=closestDepth) {
				shadow += 1.0;
			}
		}
		shadow /= float(samples);
		shadow = (1.0f-shadow);
	}
	return shadow;
}

float computeShadowFactor() {
	if(!shadowEnabled) {
		return 1.0f;
	}

	float factor = 0.0f;
	int nullShadow = 0;
	int shadowMapIndex = 0;
	int shadowCubeMapIndex = 0;

	for(int i = 0; i<MAX_SHADOW && i<NumShadow; i++) {
		if(ShadowType[i]==POINT_SHADOW) {
			shadowCubeMapIndex++;
		} else {
			shadowMapIndex++;
		}

		if(Lights[LightIndex[i]].shadowType==NO_SHADOW) {
			nullShadow++;
			continue;
		}

		if(ShadowType[i]==POINT_SHADOW) {	// Point shadows	
			factor += computeShadowCubeMap(i, shadowCubeMapIndex-1);
		} else {							// Directional or spot shadows
			factor += computeShadowMap(i, shadowMapIndex-1);
		}
	}

	return nullShadow!=NumShadow ? factor/NumShadow : 1.0f;
}

vec3 applyFog(vec3  rgb,      // original color of the pixel
	vec3  rayDir)  // camera to point vector
{
	float fogAmount = 0.0f;
	float viewDistance = distance(EyePosition, vsOutput.Position);
	float factor = 0.1f;

	if(FogType==FOG_LINEAR) {
		//Linear
		fogAmount = (80-viewDistance)/(80-10);
		fogAmount = 1-clamp(fogAmount, 0.0, 1.0);
	} else if(FogType==FOG_EXPONENTIAL) {
		//Exponential fog
		fogAmount = 1.0/exp(factor * viewDistance * FogDensity);
		fogAmount = 1-clamp(fogAmount, 0.0, 1.0);
	} else if(FogType==FOG_EXPONENTIAL_SQUARED) {
		//Exponential squared fog
		fogAmount = 1.0/exp((factor * viewDistance * FogDensity)* (factor * viewDistance * FogDensity));
		fogAmount = 1-clamp(fogAmount, 0.0, 1.0);
	} else if(FogType==FOG_ALTITUDE) {
		//Altitude fog
		float b = FogDensity*10;
		float c = FogDensity*10;
		fogAmount = c*exp(-EyePosition.y*b)*(1.0-exp(-viewDistance*rayDir.y*b))/rayDir.y;
		fogAmount = clamp(fogAmount, 0.0, 1.0);
	}

	return mix(rgb, FogColor.rgb, fogAmount);
}

void main() {
	// Normal.
	vec3 ComputedNormal = computeBumpedNormal();

	// Texture coordinates.
	vec2 texCoords = (vsOutput.TexCoord.xy*material.tc_scale)+material.tc_offset;
	
	// Textures.
	vec4 albedoColor = vec4(1.0f);
	if(useAlbedoMap) {
		albedoColor = texture(AlbedoMap, texCoords);
	}

	vec4 specularColor = vec4(1.0f);
	if(useSpecularMap) {
		specularColor = texture2D(SpecularMap, texCoords);
	}

	float shininessValue = 1.0f;
	if(useShininessMap) {
		shininessValue = texture2D(ShininessMap, texCoords).r;
	} else {
		shininessValue = material.shininess;
	}

	vec4 ambientColor = vec4(1.0f);
	if(useAmbientMap) {
		ambientColor = texture2D(AmbientMap, texCoords);
	}

	vec4 emissiveColor = vec4(-1.0f);
	if(useEmissiveMap) {
		emissiveColor = texture2D(EmissiveMap, texCoords);
	}

	float opacityValue = 1.0f;
	if(useOpacityMap) {
		opacityValue = texture2D(OpacityMap, texCoords).a;
	} else {
		opacityValue = material.opacity;
	}

	vec4 aoColor = vec4(1.0f);
	if(useAOMap) {
		aoColor *= texture2D(AOMap, computeScreenTexCoord());
	}

	vec4 ReflectionColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	vec4 RefractionColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	if(useSkybox) {
		vec3 I = normalize(vsOutput.Position-EyePosition);

		// Reflection.
		vec3 Reflection_Vector = reflect(I, normalize(vsOutput.Normal));
		ReflectionColor = texture(Skybox, Reflection_Vector)*material.reflection;

		// Refraction.
		float ratio = 1.00/material.refraction;
		vec3 Refraction_Vector = refract(I, normalize(vsOutput.Normal), ratio);
		RefractionColor = texture(Skybox, Refraction_Vector);
	}

	// Lights.
	vec4 ambientLight = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vec4 diffuseLight = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vec4 specularLight = vec4(0.0f, 0.0f, 0.0f, 1.0f);

	// EyeToFrag
	vec3 viewVector = EyePosition-vsOutput.Position;
	vec3 viewDirection = normalize(viewVector);
	float lenghtViewVector = length(viewVector);

	if(Shadeless) { // If shadeless avoid light computation.
		ambientLight += (ambientColor * aoColor.r * material.ambient);
		diffuseLight += (albedoColor * material.diffuse);
		specularLight += (specularColor * material.specular);
	} else {
		int shadowMapIndex = 0;
		int shadowCubeMapIndex = 0;
		int shadowIndex = 0;
		float shadowFactor = 1.0f;	// Shadow factor.

		for(int i = 0; i<NumLights; i++) {
			vec3 lightDirection = -Lights[i].direction;
			float attenuation = 1.0f;

			if(Lights[i].isLocal) {
				lightDirection = normalize(Lights[i].position-vsOutput.Position);
				float lightDistance = distance(Lights[i].position, vsOutput.Position);

				attenuation = 1.0f/(Lights[i].constant_attenuation
					+Lights[i].linear_attenuation * lightDistance
					+Lights[i].exponential_attenuation * lightDistance * lightDistance);

				if(Lights[i].isSpot) {
					float tetha = dot(lightDirection, -Lights[i].direction);

					if(tetha>Lights[i].cutoff) {
						attenuation *= (1.0-(1.0-tetha) * 1.0/(1.0-Lights[i].cutoff));
					} else {
						attenuation = 0.0f;
					}
				}
			}

			float diffuse = max(0.0, dot(ComputedNormal, lightDirection)) * Lights[i].diffuse_intensity;

			//vec3 viewDir = normalize(EyePosition-vsOutput.Position);
			vec3 reflectDir = reflect(-lightDirection, ComputedNormal);
			float specular = pow(max(dot(viewDirection, reflectDir), 0.0), shininessValue) * Lights[i].specular_strength;

			// Computing shadow factor.
			//shadowFactor = computeShadowFactor();

			shadowFactor = 1.0f;
			if(Lights[LightIndex[i]].shadowType!=NO_SHADOW) {
				if(ShadowType[i]==POINT_SHADOW) {	// Point shadows	
					shadowFactor = computeShadowCubeMap(i, shadowCubeMapIndex++);
				} else {							// Directional or spot shadows
					shadowFactor = computeShadowMap(i, shadowMapIndex++);
				}
			}

			ambientLight += (ambientColor * aoColor.r * material.ambient * Lights[i].color * attenuation * Lights[i].ambient_intensity);
			diffuseLight += (albedoColor * material.diffuse * Lights[i].color * diffuse * attenuation * shadowFactor);
			specularLight += (specularColor * material.specular * Lights[i].color * specular * attenuation * shadowFactor);
		}
	}

	vec4 totalLight = ambientLight+diffuseLight+specularLight;
	vec4 substateColor = mix(MinColor, MaxColor, vsOutput.SubstateColorRatio);	// Apply the min/max color ratio.

	FragColor = substateColor*totalLight+ReflectionColor*RefractionColor;

	BloomColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	if(emissiveColor.a>0.0f) {
		FragColor += material.emissive;
		BloomColor = material.emissive;
	}

	FragColor.a = clamp(substateColor.a*opacityValue, 0.0f, 1.0f);

	// Apply Fog.
	if(FogActive) {
		FragColor.rgb = applyFog(FragColor.rgb, viewDirection);
	}

	// Gamma correction if HDR and OIT are disabled.
	if(!ActiveHDR) {
		FragColor.rgb = pow(FragColor.rgb, vec3(1.0/Gamma));
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
		item.w = 0; // packUnorm4x8(BloomColor);

		imageStore(list_buffer, int(index), item);
	}

}
