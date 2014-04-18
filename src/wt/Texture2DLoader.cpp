/**
 * @file Texture2DLoader.cpp
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */

#include "wt/stdafx.h"
#include "wt/Texture2DLoader.h"

#define TD_TRACE_TAG "Texture2DLoader"

namespace wt
{

void Texture2DLoader::load(AIOStream* stream, Texture2D* texture){
	// Create an OpenGL texture
	texture->create();

	// Load an image
	Image tmpImage;
	DevilImageLoader::getSingleton().load(stream, &tmpImage);

	// Upload the image data to the texture
	texture->setData(&tmpImage);
}

void Texture2DLoader::save(AIOStream*, Texture2D*){
	TRACEW("Not implemented");
}

void Texture2DLoader::create(Texture2D*){
}

	
} // </wt>

