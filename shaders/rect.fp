#version 330

in vec2 fsTexCoord;

out vec4 outFragColor;

// WARNING CHANGED FROM samplerRect to sampler2D for compability reason, probably broken
uniform sampler2D uRectImage;

void main(){
 	outFragColor = texture(uRectImage, fsTexCoord);
}