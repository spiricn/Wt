#ifndef WT_ARENDERER_H
#define WT_ARENDERER_H


namespace wt{

namespace math{

class Camera;

}; // </math>

class Scene;

namespace gl{
	class ShaderProgram;
};

class ARenderer{
public:
	enum PassType{
		ePASS_NONE = -1,
		ePASS_NORMAL = 0,
		ePASS_GODRAY,
		ePASS_SHADOW,
	};

	ARenderer(){
	}

	virtual ~ARenderer(){
	}

	
	virtual gl::ShaderProgram* setupSceneLighting(){
		return NULL;
	}

	virtual void onSceneLightingChanged(Scene* scene){
	}

	virtual void onSceneFogChanged(Scene* scene){
	}

	virtual void render(Scene* scene, math::Camera* camera, PassType pass) = 0;

	virtual void create() = 0;
};

}; // </wt>

#endif // </WT_ARENDERER_H>