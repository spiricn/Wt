#ifndef WT_OFFSCREENDEMO_H
#define WT_OFFSCREENDEMO_H

#include "demo/ADemo.h"

#include "wt/RenderTarget.h"

namespace wt{

class OffscreenDemo : public ADemo{
private:
	RenderTarget mRenderTarget;
	Texture2D mOffscreenTexture;

public:

	OffscreenDemo(){
	}

	void onRender(float dt){
		// Ofscreen rendering
		// TODO for some reason this isn't rendering if main screen rendering is commented out
		getRenderer()->render( *getScene(), &mRenderTarget);

		// Main screen rendering
		getRenderer()->render( *getScene() );

		//mOffscreenTexture.dump("test3.jpg");

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);


		// TODO fix this
		Texture2D tmp;
		tmp.create();
		tmp.setData(
			getManager()->getWindow()->getWidth(), getManager()->getWindow()->getHeight(),
			GL_RGB, GL_RGB, NULL, GL_UNSIGNED_BYTE, true);

		Texture2D::copy(&mOffscreenTexture, &tmp);
	
		getRenderer()->render(
			&tmp,
			glm::vec2(getManager()->getWindow()->getWidth(), getManager()->getWindow()->getHeight()),
			0, 0,
			300, 300
		);
	}

	void onUpdate(float dt){
		getPhysics()->update(dt);
		getScene()->update(dt);
		getCameraControl()->handle(dt, getManager()->getInput());
	}

	void onKeyDown(VirtualKey c){
		ADemo::onKeyDown(c);
	}

	void onStart(const LuaObject& config){
		getCameraControl()->setCamera(&getScene()->getCamera());


		mOffscreenTexture.create();
		

		mOffscreenTexture.setData(
			getManager()->getWindow()->getWidth(), getManager()->getWindow()->getHeight(),
			GL_RGB, GL_RGB, NULL, GL_UNSIGNED_BYTE, false);

		mRenderTarget.getFrameBuffer().create();

		mRenderTarget.getFrameBuffer().addAttachment(GL_COLOR_ATTACHMENT0, &mOffscreenTexture);

		mRenderTarget.addBuffer(GL_COLOR_ATTACHMENT0);

		WT_ASSERT(mRenderTarget.getFrameBuffer().isComplete(), "Incomplete framebuffer");
	}

	String getConfigFile() const{
		return "";
	}

	String getLevelFile() const{
		return "world.lua";
	}

}; // </OffscreenDemo>

WT_DECLARE_DEMO(OffscreenDemo)

}; // </wt>

#endif // </WT_OFFSCREENDEMO_H>
