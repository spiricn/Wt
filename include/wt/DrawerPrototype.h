#ifndef WT_DRAWERPROTOTYPE_H
#define WT_DRAWERPROTOTYPE_H

#include <wt/stdafx.h>

#include <wt/Texture2D.h>
#include <wt/Buffer.h>
#include <wt/Renderer.h>

namespace wt{


float noise1(int32_t x, int32_t y){
    int32_t n = x + y * 57;
    n = (n<<13) ^ n;
    return ( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);    
}

float noise2(int32_t x, int32_t y){
    int32_t n = x + y * 57;
    n = (n<<13) ^ n;
    return ( 1.0 - ( (n * (n * n * 15737 + 789227) + 131007407) & 0x7fffffff) / 1073741824.0);    
}


float noise3(int32_t x, int32_t y){
    int32_t n = x + y * 57;
    n = (n<<13) ^ n;
    return ( 1.0 - ( (n * (n * n * 15739 + 789311) + 131005793) & 0x7fffffff) / 1073741824.0);    
}


float noise4(int32_t x, int32_t y){
    int32_t n = x + y * 57;
    n = (n<<13) ^ n;
    return ( 1.0 - ( (n * (n * n * 15749 + 789419) + 131007397) & 0x7fffffff) / 1073741824.0);    
}


typedef float(*NoiseFuncPtr)(int32_t,int32_t);

float smoothNoise(int32_t x, int32_t y, NoiseFuncPtr noise){

	float corners = (noise(x-1, y-1) + noise(x+1, y-1) + noise(x+1, y+1) + noise(x-1, y+1)) / 16;
	float sides = (noise(x-1, y) + noise(x+1, y) + noise(x, y-1) + noise(x, y+1)) / 8;
	float center = noise(x, y) / 4;

	return corners + sides + center;
}

float interpolatedNoise(float x, float y, NoiseFuncPtr noise){
	int32_t intX = (int32_t)x; // ceo broj
	float fracX = x - intX; // ostatak

	int32_t intY = (int32_t)y; // ceo broj
	float fracY = y - intY; // ostatak


	float v1 = smoothNoise(intX, intY, noise);
	float v2 = smoothNoise(intX+1, intY, noise);
	float v3 = smoothNoise(intX, intY+1, noise);
	float v4 = smoothNoise(intX+1, intY+1, noise);

	float i1 = math::cosineInterpolation(v1, v2, fracX);
	float i2 = math::cosineInterpolation(v3, v4, fracX);

	return math::cosineInterpolation(i1, i2, fracY);
}

float perlinNoise(float x, float y){
	float total = 0.0f;

	float persistance = 1/4;
	int numOctaves = 1;

	NoiseFuncPtr noises[4] = {
		noise1,
		noise2,
		noise3,
		noise4
	};
	
	for(int i=0; i<4; i++){
		float freq = pow(2.0f, (float)i);
		float amplitude = pow(persistance, (float)i);

		total += interpolatedNoise(x*freq, y*freq, noises[i]);
	}

	return total;
}

class DrawerPrototype{
private:
	Texture2D mTexture;

	uint32_t mWidth, mHeight;
public:
	DrawerPrototype(uint32_t w, uint32_t h) : mWidth(w), mHeight(h){
	}

	void create(){
		mTexture.create();
		Buffer<GLbyte> pixels(mWidth*mHeight*3);
		pixels.clearMem(0x00);

		mTexture.setData(mWidth, mHeight, GL_RGB, GL_RGB, pixels.getData(), GL_UNSIGNED_BYTE);
	}

	void process(){
		for(uint32_t x=0; x<mWidth; x++){
			for(uint32_t y=0; y<mHeight; y++){
				// TODO
			}
		}
	}

	void setPixel(int x, int y, float val){
		GLbyte rgb[] = {val*255.0, val*255.0, val*255.0};
		mTexture.setSubData(
			x, y, 1, 1, rgb);
	}


	void render(Renderer* renderer){
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		glClearColor(1.0, 0.0, 0.0, 1.0);
		glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
		renderer->render(&mTexture, glm::vec2(mWidth, mHeight), 0, 0, mWidth, mHeight);
	}

}; // </DrawerPrototype>

}; // </wt>

#endif // </WT_DRAWERPROTOTYPE_H>