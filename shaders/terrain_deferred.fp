#version 130
#extension GL_EXT_gpu_shader4 : enable

#define eNORMAL_PASS 0
#define eGODRAY_PASS 1
#define eSHADOW_PASS 2

uniform sampler2DArray texArray;
uniform sampler2D map;
uniform vec4 uFogColor;
uniform int uPassType;
in vec3 fsWorldPos;
in vec3 fsNormal;

in vec2 fsTexCoords;
in vec2 fsTiledTexCoords;

out vec3 outWorldPos;
out vec3 outDiffuse;
out vec3 outNormal;

void main(){

	// Map sample
	vec4 mapSample = texture(map, fsTexCoords);
	float inverse = 1.0f/(mapSample.r + mapSample.g + mapSample.b);

	// Factor in individual textures
	outDiffuse = vec3(0,0,0);

	vec4 texSample;
	for(int i=0; i<3; i++){
		texSample = texture2DArray(texArray, vec3(fsTiledTexCoords*50, i));
		texSample *= mapSample[i]*inverse;
		outDiffuse += texSample.xyz;
	}

	outNormal = normalize(fsNormal);

	outWorldPos = fsWorldPos;
}