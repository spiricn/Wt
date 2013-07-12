#version 330

in vec2 fsTexCoord;

out vec4 outFragColor;

// WARNING CHANGED FROM samplerRect to sampler2D for compability reason, probably broken
uniform sampler2D uRectImage;
uniform vec2 uLightPositionOnScreen;

#define NUM_SAMPLES 100
#define EXPOSURE 0.009
#define DECAY 1.0
#define WEIGHT 3.1
#define DENSITY 0.84
#define ILLUMINATION_DECAY 1.0

void main(){
	// distance from the current texel to screen light position
	vec2 deltaTextCoord = vec2( fsTexCoord.st - uLightPositionOnScreen.xy );

 	vec2 textCoo = fsTexCoord.st;

	// step
 	deltaTextCoord *= 1.0 /  float(NUM_SAMPLES) * DENSITY;

 	float illuminationDecay = 1.0;

 	for(int i=0; i < NUM_SAMPLES ; i++){
		// move closer to the light & sample a texel
    	textCoo -= deltaTextCoord;

    	vec4 sample = texture(uRectImage, textCoo);

    	sample *= illuminationDecay * WEIGHT;
    			
    	outFragColor += sample;

    	illuminationDecay *= DECAY;

 	}

 	outFragColor *= EXPOSURE;
}