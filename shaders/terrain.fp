#version 130
#extension GL_EXT_gpu_shader4 : enable

#pragma include shaders/lighting.fp

#define eNORMAL_PASS 0
#define eGODRAY_PASS 1
#define eSHADOW_PASS 2

uniform sampler2DArray texArray;
uniform sampler2D map;
uniform vec4 uFogColor;
uniform int uPassType;

in vec2 fsTexCoords;
in vec2 fsTiledTexCoords;

out vec4 outFragColor;

void main(){

	if(uPassType != eNORMAL_PASS){
		outFragColor = vec4(0, 0, 0, 0);
	}
	else{
		// terrain fragment
		// Map sample
		vec4 mapSample = texture(map, fsTexCoords);
		float inverse = 1.0f/(mapSample.r + mapSample.g + mapSample.b);

		// Factor in individual textures
		outFragColor = vec4(0,0,0,0);
		vec4 texSample;
		for(int i=0; i<3; i++){
			texSample = texture2DArray(texArray, vec3(fsTiledTexCoords*50, i)
			);
			texSample *= mapSample[i]*inverse;
			outFragColor += texSample;
		}

		vec3 normal = normalize(fsNormal);

		// lighting
		vec4 lightColor = calculateLight(uDirectionalLight.color, uDirectionalLight.ambientItensity,
			uDirectionalLight.diffuseItensity, uDirectionalLight.direction);

		for(int i=0; i<uNumPointLights; i++){
			lightColor += calculatePointLight(uPointLights[i]);
		}

		for(int i=0; i<uNumSpotLights; i++){
			lightColor += calculateSpotLight(uSpotLights[i]);
		}

		outFragColor = calcFog(outFragColor * lightColor);
	}
}