#version 330

in vec3 fsWorldPos;
in vec3 fsNormal;
in vec2 fsTexCoord;

out vec3 outWorldPos;
out vec3 outDiffuse;
out vec3 outNormal;
out vec3 outTexCoord;

uniform sampler2D uTextureSampler;

void main(void) {
	outWorldPos = fsWorldPos;

	outDiffuse = texture(uTextureSampler, fsTexCoord).xyz;

	outNormal = normalize(fsNormal);

	outTexCoord = vec3(fsTexCoord, 0.0);
}