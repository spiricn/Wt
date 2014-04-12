#version 330

in vec2 fsTexCoord;

out vec4 outFragColor;

uniform sampler2D uTextureSampler;
uniform vec4 uColor;
uniform bool uTextured;

void main(){
	vec4 color;
	if(uTextured){
 		color = texture(uTextureSampler, fsTexCoord) * uColor;
	}
	else{
		color = uColor;
	}

	outFragColor = color;
}