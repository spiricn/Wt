#ifndef WT_LIGHTING_FP
#define WT_LIGHTING_FP

#define MAX_POINT_LIGHTS	( 20 )
#define MAX_SPOT_LIGHTS		( 20 )


struct DirectionalLight{
	vec4 color;
	float ambientItensity;
	float diffuseItensity;
	vec3 direction;
}; // </DirectionalLight>

struct Attenuation{
	float linear;
	float constant;
	float exponential;
}; // </Attenuation>

struct PointLight{
	vec4 color;
	float ambientItensity;
	float diffuseItensity;
	vec3 position;
	Attenuation attenuation;
}; // </PointLight>

struct SpotLight{
	PointLight base;
	vec3 direction;
	float cutoff;
}; // </SpotLight>

struct Material{
	vec4 ambientColor;
	vec4 diffuseColor;
	vec4 specularColor;
	float shininess;
}; // </Material>

struct Fog{
	float density;
	vec4 color;
	int enabled;
}; // </Fog>

struct Lighting{
	DirectionalLight directionalLight;
	PointLight pointLights[MAX_POINT_LIGHTS];
	SpotLight spotLights[MAX_SPOT_LIGHTS];
	int numPointLights;
	Material material;
	Fog fog;
	vec3 eyePos;
	int numSpotLights;
	sampler2D shadowMap;
	int shadowMappingEnabled;
}; // </Lighting>

// Structs cannot contain variables of sampler types
uniform mat4 uLighting_depthBiasLightMVP;
uniform Lighting uLighting;

vec4 calculateLight(vec3 normal, vec3 worldPos, vec4 color, float ambientIntensity,
	float diffuseItensity, vec3 direction, float shadowFactor){
	vec4 ambientColor = color * ambientIntensity;
	vec4 specularColor = vec4(0, 0, 0, 0);
	vec4 diffuseColor = vec4(0, 0, 0, 0);

	/*
		calculate the cosine of the angle between the light vector and the normal by doing a dot product

		The normal passed from the vertex shader is normalized before it is used.
		This is because the interpolation the vector went through may have changed its length and it is no longer a unit vector.
	*/
	float diffuseFactor = dot(normal, -direction);
	
	if(diffuseFactor > 0){
		diffuseColor = color * diffuseItensity * diffuseFactor;

		// Drection vertex -> camera
		vec3 VertexToEye = normalize(uLighting.eyePos - worldPos);

        vec3 LightReflect = normalize(reflect(direction, normal));
        float SpecularFactor = dot(VertexToEye, LightReflect);
        SpecularFactor = pow(SpecularFactor, uLighting.material.shininess);
		
        if (SpecularFactor > 0) {
            specularColor = color * uLighting.material.specularColor * SpecularFactor;
        }
	}
	else{
		/*
		If the diffuse factor is negative or equals to zero it means that light strikes the surface at an obtuse angle
		in this case the diffuse light has no effect on the surface
		*/
		diffuseColor = vec4(0, 0, 0, 0);
	}

	// TODO shouldn't affect ambient
	return shadowFactor * ( ambientColor*uLighting.material.ambientColor +  ( diffuseColor*uLighting.material.diffuseColor + specularColor) );
}

float calculateShadowFactor(vec3 worldPos){
	vec4 ShadowCoord = uLighting_depthBiasLightMVP * vec4(worldPos, 1);
	vec3 projCoords = ShadowCoord.xyz / ShadowCoord.w;

	float bias = 0.0001;
	float visibility = 1;
	if ( texture( uLighting.shadowMap, ShadowCoord.xy/ShadowCoord.w ).z  <  (ShadowCoord.z-bias)/ShadowCoord.w){
		visibility = 0.5f;
	}

	return visibility;
}

vec4 calculateDirectionalLight(vec3 normal, vec3 worldPos){
	float shadowFactor = 1.0f;
	if(uLighting.shadowMappingEnabled == 1){
		shadowFactor = calculateShadowFactor(worldPos);
	}

	return calculateLight(normal, worldPos, uLighting.directionalLight.color, uLighting.directionalLight.ambientItensity,
			uLighting.directionalLight.diffuseItensity, uLighting.directionalLight.direction, shadowFactor);
}

vec4 calculatePointLight(PointLight light, vec3 normal, vec3 worldPos){
	// direction to the point light
	vec3 direction = worldPos - light.position;

	// distance from the point light
	float d = length(direction);

	direction = normalize(direction);

	float shadowFactor = 1.0f;
	if(uLighting.shadowMappingEnabled == 1){
		shadowFactor = calculateShadowFactor(worldPos);
	}

	vec4 color = calculateLight(normal, worldPos, light.color, light.ambientItensity, light.diffuseItensity, direction, shadowFactor);

	float att = light.attenuation.constant + light.attenuation.linear * d + light.attenuation.exponential * d * d;

	return color / att;

}

vec4 calculatePointLight(int index, vec3 normal, vec3 worldPos){
	return calculatePointLight(uLighting.pointLights[index], normal, worldPos);
}

vec4 calculateSpotLight(SpotLight light, vec3 normal, vec3 worldPos){
	vec3 lightToPixel = normalize(worldPos - light.base.position);

    float spotFactor = dot(lightToPixel, light.direction);

    if (spotFactor > light.cutoff) {
        vec4 color = calculatePointLight(light.base, normal, worldPos);
        return color * (1.0 - (1.0 - spotFactor) * 1.0/(1.0 - light.cutoff));
    }
    else{
        return vec4(0,0,0,0);
    }
}

vec4 calcFog(vec4 color, vec3 worldPos){
	if(uLighting.fog.enabled == 0){
		return color;
	}

	// distance from the camera eye
	float d = abs(distance(worldPos, uLighting.eyePos));

	float factor = clamp(
		1.0 - exp(-uLighting.fog.density*d), 0.0, 1.0
	);

	// mix the fragment color with the fog
	return mix(color, vec4(uLighting.fog.color.rgb, 1.0), factor);
}

#endif