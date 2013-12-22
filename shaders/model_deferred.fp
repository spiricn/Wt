#version 330

in vec3 fsWorldPos;
in vec3 fsNormal;
in vec2 fsTexCoord;
in vec3 fsTangent;

out vec3 outWorldPos;
out vec3 outDiffuse;
out vec3 outNormal;
out vec4 outGodray;

uniform sampler2D uTextureSampler;
uniform sampler2D uNormalMap;
uniform int uUseNormalMap;
uniform int uUseAlphaTest;

vec3 calcBumpedNormal(){
	// Could probably avoid normalizing these
    vec3 normal = normalize(fsNormal);
    vec3 tangent = normalize(fsTangent);

	// Calculate the bitangent
    tangent = normalize(tangent  - dot(tangent, normal) * normal);
    vec3 bitangent = cross(tangent, normal);

    vec3 bumpMapNormal = texture(uNormalMap, fsTexCoord).xyz;

    bumpMapNormal = 2.0 * bumpMapNormal - vec3(1.0, 1.0, 1.0);

    mat3 TBN = mat3(tangent, bitangent, normal);

    vec3 newNormal = TBN * bumpMapNormal;

    return normalize(newNormal );
}

void main(void) {
	outWorldPos = fsWorldPos;

	vec4 sample = texture(uTextureSampler, fsTexCoord);
	
	if(uUseAlphaTest == 1 &&  sample.a < 0.2){
		discard;
	}

	outDiffuse = sample.rgb;

	if(uUseNormalMap == 1){
		outNormal = calcBumpedNormal();
	}
	else{
		outNormal = normalize(fsNormal);
	}
	
	outGodray = vec4(0, 0, 0, 1);
}