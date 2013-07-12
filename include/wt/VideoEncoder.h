#ifdef REVEL_ENABLED
#ifndef WT_VIDEOENCODER_H
#define WT_VIDEOENCODER_H


#include "wt/stdafx.h"


#include "revel.h"
#include "wt/Exception.h"
#include "wt/Buffer.h"

//#ifdef _DEBUG
#pragma comment(lib, "Revel.lib")
//#endif

namespace wt{

class VideoEncoder{

private:
	Revel_Params mParams;
	int mHandle;
	bool mIsEncoding;
public:

	VideoEncoder() : mIsEncoding(false){
		Revel_InitializeParams(&mParams);

		mParams.quality = 1.0f;
		mParams.codec = REVEL_CD_XVID;
		mParams.hasAudio = 0;
	}

	void start(const String& path){
		if(Revel_CreateEncoder(&mHandle)!=REVEL_ERR_NONE){
			WT_EXCEPT("VideoEncoder", "Error initializing encoder");
		}

		if(Revel_EncodeStart(mHandle, path.c_str(), &mParams)!=REVEL_ERR_NONE){
			WT_EXCEPT("VideoEncoder", "Error starting file encoding");
		}

		mIsEncoding=true;
	}

	void setFrameRate(float fps){
		mParams.frameRate = fps;
	}

	void encodeFrame(const char* bfr, uint32_t width, uint32_t height, bool flip=true){
		Revel_VideoFrame frame;
		frame.width = width;
		frame.height = height;
		frame.bytesPerPixel = 3;
		frame.pixelFormat = REVEL_PF_BGR;
		
		int err;
		int frameSize;

		const char* data = bfr;

		if(flip){
			char* tmp = new char[width*height*3];
			memcpy(tmp, bfr, width*height*3);

			for(uint32_t row=0; row<height; row++){
				for(uint32_t col=0; col<width; col++){
				
					uint32_t src = (row*width + col)*3;
					uint32_t dst = (((height-1)-row)*width + col)*3; 
				
					memcpy((void*)&tmp[src], (void*)&bfr[dst], 3);
				}
			}

		
			data = tmp;

		}
		else{
			data = bfr;
		}

		frame.pixels = (void*)data;

		if((err=Revel_EncodeFrame(mHandle, &frame, &frameSize))!=REVEL_ERR_NONE){
			WT_EXCEPT("VideoEncoder", "Error encoding frame %d", err);
		}


		if(flip){
			delete[] data;
		}
	}

	void setSize(uint32_t width, uint32_t height){
		mParams.width = width;
		mParams.height = height;
	}

	bool isEncoding()const{
		return mIsEncoding;
	}

	void finish(){
		if(Revel_EncodeEnd(mHandle)!=REVEL_ERR_NONE){
			WT_EXCEPT("VideoEncoder", "Error finishing encoding");
		}
		Revel_DestroyEncoder(mHandle);

		mIsEncoding=false;
	}
};

}; // </wt>

#endif

#endif