#ifndef WT_LIGHTING_FP
#define WT_LIGHTING_FP
// TODO put all the modules uniforms in a struct
in vec3 fsNormal;
in vec3 fsWorldPos;


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
}; // </Fog>

#define MAX_POINT_LIGHTS 3
#define MAX_SPOT_LIGHTS 3

uniform DirectionalLight uDirectionalLight;
uniform PointLight uPointLights[MAX_POINT_LIGHTS];
uniform SpotLight uSpotLights[MAX_SPOT_LIGHTS];
uniform int uNumPointLights;
uniform Material uMaterial;
uniform Fog uFogParams;
uniform vec3 uEyePos;
uniform int uNumSpotLights;

vec4 calculateLight(vec4 color, float ambientIntensity, float diffuseItensity, vec3 direction){
	vec4 ambientColor = color * ambientIntensity;
	vec4 SpecularColor;
	/*
		calculate the cosine of the angle between the light vector and the normal by doing a dot product

		The normal passed from the vertex shader is normalized before it is used.
		This is because the interpolation the vector went through may have changed its length and it is no longer a unit vector.
	*/
	float diffuseFactor = dot(fsNormal, -direction);

	vec4 diffuseColor;
	if(diffuseFactor > 0){
		diffuseColor = color * diffuseItensity * diffuseFactor;

		vec3 VertexToEye = normalize(uEyePos - fsWorldPos);
        vec3 LightReflect = normalize(reflect(direction, fsNormal));
        float SpecularFactor = dot(VertexToEye, LightReflect);
        SpecularFactor = pow(SpecularFactor, uMaterial.shininess);
		
        if (SpecularFactor > 0) {
            SpecularColor = color * uMaterial.specularColor * SpecularFactor;
        }
	}
	else{
		/*
		If the diffuse factor is negative or equals to zero it means that light strikes the surface at an obtuse angle
		in this case the diffuse light has no effect on the surface
		*/
		diffuseColor = vec4(0, 0, 0, 0);
	}

	return ambientColor*uMaterial.ambientColor + diffuseColor*uMaterial.diffuseColor + SpecularColor;
}


vec4 calculatePointLight(PointLight light){
	// direction to the point light
	vec3 direction = fsWorldPos - light.position;

	// distance from the point light
	float d = length(direction);

	direction = normalize(direction);

	vec4 color = calculateLight(light.color, light.ambientItensity, light.diffuseItensity, direction);

	float attenuation = light.attenuation.constant +
		light.attenuation.linear * d +
		light.attenuation.exponential * d * d;

	return color/attenuation;
}


vec4 calculateSpotLight(SpotLight light){
	vec3 lightToPixel = normalize(fsWorldPos - light.base.position);

    float spotFactor = dot(lightToPixel, light.direction);

    if (spotFactor > light.cutoff) {
        vec4 color = calculatePointLight(light.base);
        return color * (1.0 - (1.0 - spotFactor) * 1.0/(1.0 - light.cutoff));
    }
    else {
        return vec4(0,0,0,0);
    }
}

vec4 calcFog(vec4 color){
	// distance from the camera eye
	float d = abs(distance(fsWorldPos, uEyePos));

	float factor = clamp(
		1.0 - exp(-uFogParams.density*d), 0.0, 1.0
	);

	// mix the fragment color with the fog
	return mix(color, vec4(uFogParams.color.rgb, 1.0), factor);
}

#endif