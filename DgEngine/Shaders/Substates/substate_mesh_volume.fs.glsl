#version 450 core

#define CODE_SUBSTATE_BYTE	0
#define CODE_SUBSTATE_INT	1
#define CODE_SUBSTATE_REAL	2

uniform mat4		MMatrix;
uniform mat4		InvMMatrix;
uniform vec3		eyePos;
uniform int			volumeTexDimension;
uniform sampler3D	volumeTex;
uniform sampler2D	frontfaceTex;
uniform int			SubstateType;
//uniform int			NoDataByte;
//uniform int			NoDataInt;
//uniform double		NoDataReal;
uniform int			MinValueByte;
uniform int			MaxValueByte;
uniform int			MinValueInt;
uniform int			MaxValueInt;
uniform double		MinValueReal;
uniform double		MaxValueReal;
uniform vec4		MinColor;
uniform vec4		MaxColor;

in vec4 vRayHit;
in vec3 EntryPoint;

layout(location = 0, index = 0) out vec4 FragColor;

void main() {
	vec3 dRay = vRayHit.xyz-eyePos;
	float distToEnd = length(dRay);
	vec3 dir = dRay/distToEnd;
	float rayTravelDist = distToEnd-texelFetch(frontfaceTex, ivec2(gl_FragCoord.xy), 0).r;

	float cellsPerUnitLength = 1.0f*volumeTexDimension;
	int numSteps = int(rayTravelDist*cellsPerUnitLength);
	vec3 rayStep = rayTravelDist*dir/numSteps;

	vec3 voxelCoord = EntryPoint;
	vec3 rayCurPos = rayStep+(vRayHit.xyz-rayTravelDist*dir);

	vec4 finalColor = vec4(0.0f);
	float ratio = 0.0f;
	for(int i = 0; i<numSteps; i++) {
		vec3 texCoords = (InvMMatrix * vec4(rayCurPos, 1.0)).xyz+vec3(0.5f);

		// Computing color ratio.
		if(SubstateType==CODE_SUBSTATE_BYTE) {
			ratio += float(texture(volumeTex, texCoords).r-MinValueByte)/float(MaxValueByte-MinValueByte);
		} else if(SubstateType==CODE_SUBSTATE_INT) {
			ratio += float(texture(volumeTex, texCoords).r-MinValueInt)/float(MaxValueInt-MinValueInt);
		} else if(SubstateType==CODE_SUBSTATE_REAL) {
			ratio += float(texture(volumeTex, texCoords).r-MinValueReal)/float(MaxValueReal-MinValueReal);
		}

		rayCurPos += rayStep;
	}

	finalColor = mix(MinColor, MaxColor, ratio);
	finalColor.a *= max(rayTravelDist, 0.0f);
	FragColor = finalColor;
}
