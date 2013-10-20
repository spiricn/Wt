#version 330

in vec3 fsWorldPos;
in vec3 fsNormal;
in vec2 fsTexCoord;

out vec3 outWorldPos;
out vec3 outDiffuse;
out vec3 outNormal;
out vec4 outGodray;

uniform sampler2D uTextureSampler;

void main(void) {
	outWorldPos = fsWorldPos;

	vec4 sample = texture(uTextureSampler, fsTexCoord);
	 
	if(sample.a < 0.2){
		discard;
	}

	outDiffuse = sample.rgb;

	outNormal = normalize(fsNormal);

	outGodray = vec4(0, 0, 0, 1);
}