
#version 130

uniform sampler2D uParticleTexture;

in vec4 fsColor;

out vec4 outFragColor;

void main(){
	outFragColor = texture(uParticleTexture, gl_PointCoord) * fsColor;
}
