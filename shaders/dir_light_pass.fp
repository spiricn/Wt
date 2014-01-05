#version 140

#pragma include shaders/lighting.fp

out vec4 outFragColor;

uniform sampler2D uPositionMap;
uniform sampler2D uColorMap;
uniform sampler2D uNormalMap;
uniform vec2 uScreenSize;

uniform mat4 uLightViewMat;
in mat4 fsLightProjMat;


void main(){
	//Convert a fragment position to normalized screen space
	// so we can use it as a texture coordinate for G-buffer sampling.
	vec2 coord = gl_FragCoord.xy / vec2(uScreenSize.x, uScreenSize.y);

	// TODO directional light is being calculated for empty pixels (i.e. nothing has been drawn there)
	// there has to be a way to avoid this

	vec3 worldPos = texture(uPositionMap, coord).xyz;
	vec3 color = texture(uColorMap, coord).xyz;
	vec3 normal = normalize(texture(uNormalMap, coord).xyz);

	
	//mat4 DepthBiasMVP = uLightViewMat;
	//vec4 ShadowCoord = DepthBiasMVP * vec4(worldPos, 1);
	//vec3 projCoords = ShadowCoord.xyz / ShadowCoord.w;
//
	//float bias = 0.0;
	//float visibility = 1;
	//if ( texture( uShadowMap, ShadowCoord.xy/ShadowCoord.w ).z  <  (ShadowCoord.z-bias)/ShadowCoord.w){
		//visibility = 0.5f;
	//}

	
	vec4 lightColor = calculateDirectionalLight(normal, worldPos);

	// TODO move this to a separate shader
	//for(int i=0; i<uNumSpotLights; i++){
		//lightColor += calculateSpotLight(uSpotLights[i], normal, worldPos);
	//}

	outFragColor = vec4(color * lightColor.xyz, 1.0);

	outFragColor = calcFog(outFragColor, worldPos);
}