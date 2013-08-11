#version 140

#pragma include shaders/lighting.fp

out vec4 outFragColor;

uniform sampler2D uPositionMap;
uniform sampler2D uColorMap;
uniform sampler2D uNormalMap;
//uniform vec2 uScreenSize;

void main(){
	//Convert a fragment position to normalized screen space
	// so we can use it as a texture coordinate for G-buffer sampling.
	vec2 coord = gl_FragCoord.xy / vec2(1280, 720);

	// TODO directional light is being calculated for empty pixels (i.e. nothing has been drawn there)
	// there has to be a way to avoid this

	vec3 worldPos = texture(uPositionMap, coord).xyz;
	vec3 color = texture(uColorMap, coord).xyz;
	vec3 normal = normalize(texture(uNormalMap, coord).xyz);

	vec4 lightColor = vec4(0.0, 0.0, 0.0, 0.0);

	for(int i=0; i<uNumPointLights; i++){
		lightColor += calculatePointLight(uPointLights[i], normal, worldPos);
	}

	outFragColor = vec4(color * lightColor.xyz, 1.0);
}