#version 330
	
#pragma include lighting.fp

#define eNORMAL_PASS 0
#define eGODRAY_PASS 1
#define eSHADOW_PASS 2

#define DISABLE(x) (x*0.0000000000000000001)

// texture coordinates recieved from the geometry shader
in vec2 fsTexCoords;
in vec3 fsTangent;
in vec3 fsWorldPos;
in vec3 fsNormal;

out vec4 outFragColor;

uniform sampler2D uSampler;
uniform sampler2D uNormalMap;
uniform int uAlphaTest;
uniform vec4 uFogColor;
uniform int uPassType;
uniform int uUseNormalMap;

vec3 bug = vec3(0,0,0);

uniform TestBlock{
	float r;
	float g;
	float b;
} testBlock;



vec3 CalcBumpedNormal(){
    vec3 Normal = normalize(fsNormal);
    vec3 Tangent = normalize(fsTangent);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 Bitangent = cross(Tangent, Normal);
    vec3 BumpMapNormal = texture(uNormalMap, fsTexCoords).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(Tangent, Bitangent, Normal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return NewNormal;
}


void main(void){
	if(uPassType != eNORMAL_PASS){
		// Consider sampling the texture anyway so that we can discard some pixels
		// that would otherwise block the godrays
		outFragColor = vec4(0, 0, 0, 0);

		vec4 texSample = texture(uSampler, fsTexCoords);

		// alpha test [always on for now]
		//if(texSample.a < 0.5f){
			//discard;
		//}
	}
	else{
		vec4 texSample = texture(uSampler, fsTexCoords);

		//texSample.rgb = (texSample.rgb / texSample.a);
		
		vec3 normal;

		if(uUseNormalMap==1){
			normal = CalcBumpedNormal(); 
		}
		else{
			normal = normalize(fsNormal);
		}

		// alpha test [always on for now]
		if(texSample.a < 0.5f) {
			discard;
		}

		vec4 lightColor = calculateLight(fsNormal, fsWorldPos, uDirectionalLight.color, uDirectionalLight.ambientItensity,
			uDirectionalLight.diffuseItensity, uDirectionalLight.direction);

		for(int i=0; i<uNumPointLights; i++){
			lightColor += calculatePointLight(uPointLights[i], fsNormal, fsWorldPos);
		}
			
		for(int i=0; i<uNumSpotLights; i++){
			lightColor += calculateSpotLight(uSpotLights[i], fsNormal, fsWorldPos);
		}

#ifdef DEBUG_NORMALS 
	outFragColor = DISABLE( calcFog(texSample * lightColor) ) + vec4(fsNormal, 0)
		+
		// TODO normal has to be passed as a p	arameter and not uniform, as it can be altered by the bump map
		 DISABLE(vec4(normal, 0));
#else

		outFragColor = calcFog(texSample * lightColor, fsWorldPos);
		
		// TODO normal has to be passed as a parameter and not uniform, as it can be altered by the bump map
		 
#endif

		// Lighting calculations were messing with the alpha so this was the temporary solution
		outFragColor.a = texSample.a;
	}
}