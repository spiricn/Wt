#version 130

out vec4 outFragColor;

uniform samplerCube uSkyboxTexture;

in vec3 fsTexCoord;

void main(){
	outFragColor = texture(uSkyboxTexture, fsTexCoord);
}