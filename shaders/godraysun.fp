#version 130

uniform sampler2D uPlanetTexture;

out vec4 outFragColor;

void main(){
	outFragColor = texture(uPlanetTexture, gl_PointCoord);
}
