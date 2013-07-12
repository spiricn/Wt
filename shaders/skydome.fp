#version 330

in vec2 fsTexCoords;
in vec3 fsPosition;

out vec4 outFragColor;

uniform sampler2D uSkyTexture;
uniform sampler2D uGlowTexture;
uniform vec3 uSunPosition;


void main(void){

	// normalized fragment position
	vec3 fragPos = normalize(fsPosition); 

	// normalized sun's position
    vec3 sunPos = normalize(uSunPosition );

	/*
		This dot product gives a large value near the sun, and a small one far away, and thus models a glow centered at the sun position
	*/
    float fragDistance = dot(fragPos, sunPos); 

    // Look up the sky color and glow colors.

    vec4 skyColor = texture2D(uSkyTexture, vec2(
		(sunPos.y + 1.0) / 2.0, // s - (sun vertical position moved to [0, 1] range from [-1 1]
		1-fragPos.y // t - (fragment's distance from the horizon
		));

    vec4 glowColor = texture2D(uGlowTexture,  vec2(
		(sunPos.y + 1.0) / 2.0, // s
		1-fragDistance)) // t
		;

    // Combine the color and glow giving the pixel value.

    outFragColor = vec4(skyColor.rgb + glowColor.rgb * glowColor.a / 2.0, skyColor.a);
}