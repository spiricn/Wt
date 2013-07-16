#version 330
	
#pragma include shaders/lighting.fp

#define eNORMAL_PASS 0
#define eGODRAY_PASS 1
#define eSHADOW_PASS 2

#define DISABLE(x) (x*0.0000000000000000001)

// texture coordinates recieved from the geometry shader
in vec2 fsTexCoords;
in vec3 fsTangent;

out vec4 outFragColor;

uniform sampler2D uSampler;
uniform sampler2D uNormalMap;
uniform int uAlphaTest;
uniform vec4 uFogColor;
uniform int uPassType;
uniform int uUseNormalMap;

vec3 bug = vec3(0,0,0);



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
		outFragColor = vec4(1, 0, 0, 0);
	}
	else{
		vec4 texSample = texture(uSampler, fsTexCoords);
		
		vec3 normal;

		if(uUseNormalMap==1){
			normal = CalcBumpedNormal();
		}
		else{
			normal = normalize(fsNormal);
		}

		// alpha test [always on for now]
		if(uAlphaTest==0 || texSample.a < 0.3f && 1==1) {
			discard;
		}

		vec4 lightColor = calculateLight(uDirectionalLight.color, uDirectionalLight.ambientItensity,
			uDirectionalLight.diffuseItensity, uDirectionalLight.direction);

		for(int i=0; i<uNumPointLights; i++){
			lightColor += calculatePointLight(uPointLights[i]);
		}
			
		for(int i=0; i<uNumSpotLights; i++){
			lightColor += calculateSpotLight(uSpotLights[i]);
		}

#ifdef DEBUG_NORMALS
	outFragColor = DISABLE( calcFog(texSample * lightColor) ) + vec4(fsNormal, 0)
		+
		// TODO normal has to be passed as a parameter and not uniform, as it can be altered by the bump map
		 DISABLE(vec4(normal, 0));
#else
		outFragColor = calcFog(texSample * lightColor)
		+
		// TODO normal has to be passed as a parameter and not uniform, as it can be altered by the bump map
		 DISABLE(vec4(normal, 0));
#endif
	}

}