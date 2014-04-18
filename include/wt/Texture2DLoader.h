/**
 * @file Texture2DLoader.h
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */
 
#ifndef WT_TEXTURE2DLOADER_H
#define WT_TEXTURE2DLOADER_H

#include "wt/Texture2D.h"

namespace wt
{

class Texture2DLoader : public Singleton<Texture2DLoader>, public AResourceLoader<Texture2D>{
public:

	void load(AIOStream* stream, Texture2D* tex);

	void save(AIOStream*, Texture2D*);

	void create(Texture2D*);

}; // </Texture2DLoader>

} // </wt>

#endif // </WT_TEXTURE2DLOADER_H>
