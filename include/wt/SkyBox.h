#ifndef WT_SKYBOX_H
#define WT_SKYBOX_H

#include "wt/stdafx.h"

#include "wt/Utils.h"
#include "wt/GLBatch.h"
#include "wt/AResource.h"
#include "wt/AResourceManager.h"
#include "wt/Image.h"
#include "wt/Singleton.h"
#include "wt/Transform.h"
#include "wt/AResourceLoader.h"
#include "wt/Serializator.h"

namespace wt
{

class SkyBox : public AResource<SkyBox>{
public:
	enum Side{
		eSIDE_POS_X,
		eSIDE_NEG_X,
		eSIDE_POS_Y,
		eSIDE_NEG_Y,
		eSIDE_POS_Z,
		eSIDE_NEG_Z
	}; // </Side>

public:
	SkyBox(AResourceManager<SkyBox>* manager=NULL, ResourceHandle handle=0, const String& name="");

	~SkyBox();

	math::Transform& getTransform();

	void bind();

	Image* getSideImage(Side side) const;

	void setImages(Image* posX, Image* negX, Image* posY, Image* negY, Image* posZ, Image* negZ);

	void create();

	void serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const;

	void deserialize(lua::State* luaState, const LuaPlus::LuaObject& src);

	gl::Batch& getBatch();

private:
	struct Vertex{
		float x;
		float y;
		float z;
	}; // </Vertex>

	struct Desc : public ASerializator<Desc>{

		struct Map : public ASerializator<Map>{
			// TODO description is of this acrhiceture to keep backward compatability
			// should switch to a nicer structure in future

			String pos_x;
			String neg_x;
			String pos_y;
			String neg_y;
			String pos_z;
			String neg_z;

			BEG_FIELDS
				FIELD(pos_x);
				FIELD(neg_x);
				FIELD(pos_y);
				FIELD(neg_y);
				FIELD(pos_z);
				FIELD(neg_z);
			END_FIELDS
		}; // </Map>

		Map map;

		BEG_FIELDS
			SER_FIELD(map);
		END_FIELDS
	}; // </Desc>

private:

	gl::Batch mBatch;
	GLuint mTexture;
	math::Transform mTransform;
	Desc mDesc;

private:
	friend class SkyBoxLoader;
	friend class SkyBoxManager;
}; // </Skybox>


class SkyBoxLoader : public AResourceLoader<SkyBox>, public Singleton<SkyBoxLoader>{
public:
	void load(AIOStream*, SkyBox*){
	}

	void save(AIOStream*, SkyBox*){
	}

	void create(SkyBox* sky){
		sky->create();
	}
}; // </SkyBoxLoader>

} // </wt>

#endif // </WT_SKYBOX_H>
