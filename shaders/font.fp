#version 130

out vec4 outFragColor;

in vec2 fsTexCoord;

uniform sampler2D uTexture;
uniform vec4 uColor;

void main(){
	outFragColor = vec4(1, 1, 1, texture(uTexture, fsTexCoord).a) * vec4(uColor.xyz, 1.0);
}