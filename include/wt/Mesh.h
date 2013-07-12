#ifndef WT_MESH_H
#define WT_MESH_H


#include "wt/stdafx.h"

#include "wt/AResourceManager.h"
#include "wt/Singleton.h"
#include "wt/Geometry.h"
#include "wt/Material.h"


namespace wt{

class Mesh{
private:
	Geometry* mGeometry;
	Material* mMaterial;
	Texture2D* mTexture;
	Texture2D* mNormalMap;
	String mName;
	bool mIsVisible;
public:
	~Mesh(){
	}

	Mesh(const String& name="") : mName(name), mMaterial(NULL),
		mGeometry(NULL), mIsVisible(true), mTexture(NULL), mNormalMap(NULL){
	}

	Texture2D* getNormalMap(){
		return mNormalMap;
	}

	void setNormalMap(Texture2D* map){
		mNormalMap = map;	
	}

	Texture2D* getTexture(){
		return mTexture;
	}

	void setTexture(Texture2D* tex){
		mTexture = tex;
	}

	void setVisible(bool state){
		mIsVisible = state;
	}

	bool isVisible() const{
		return mIsVisible;
	}

	const String getName() const{
		return mName;
	}

	void setGeometry(Geometry* geometry){
		mGeometry = geometry;
	}

	void setMaterial(Material* material){
		mMaterial = material;
	}

	Geometry* getGeometry(){
		return mGeometry;
	}

	Material* getMaterial(){
		return mMaterial;
	}
};

}; // </wt>

#endif
