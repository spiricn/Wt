#ifndef WT_ARENDERER_H
#define WT_ARENDERER_H


namespace wt
{

namespace math
{

class Camera;

} // </math>


namespace gl
{
	class ShaderProgram;
} // </gl>

class Scene;
class Renderer;
class Texture2D;

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

	virtual void onSceneLightingChanged(Scene* scene, Renderer*){
	}

	virtual void onSceneFogChanged(Scene* scene, Renderer*){
	}

	virtual void render(Scene* scene, math::Camera* camera, PassType pass, Texture2D* shadowMap) = 0;

	virtual void create() = 0;

	virtual bool isDeferred() const = 0;
};

}; // </wt>

#endif // </WT_ARENDERER_H>