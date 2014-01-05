#ifndef WT_SKYDOMERENDERER_H
#define WT_SKYDOMERENDERER_H

#include "wt/ARenderer.h"

namespace wt
{
	
class SkyDomeRenderer : public ARenderer{
public:
	SkyDomeRenderer();

	~SkyDomeRenderer();

	void create();

	void render(Scene* scene, math::Camera* camera, PassType pass, Texture2D* shadowMap);

	bool isDeferred() const;

private:
}; // </SkyDomeRenderer>

} // </wt>

#endif // </WT_SKYDOMERENDERER_H>
