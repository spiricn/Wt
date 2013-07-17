#version 130

uniform sampler2D uPlanetTexture;
uniform vec4 uSourceColor;

out vec4 outFragColor;

void main(){
	outFragColor = texture(uPlanetTexture, gl_PointCoord) * vec4(uSourceColor.rgb, 1);
}
