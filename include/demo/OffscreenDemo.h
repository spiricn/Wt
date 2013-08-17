#ifndef WT_OFFSCREENDEMO_H
#define WT_OFFSCREENDEMO_H

#include "demo/ADemo.h"

#include "wt/RenderTarget.h"
#include "wt/RenderBuffer.h"

namespace wt{

class OffscreenDemo : public ADemo{
private:
	RenderTarget mRenderTarget;
	Texture2D mOffscreenTexture;
	gl::RenderBuffer mDepthBuffer;

public:

	OffscreenDemo(){
	}

	void onRender(float dt){
		// Ofscreen rendering
		// TODO for some reason this isn't rendering if main screen rendering is commented out
		

		// Main screen rendering
		getRenderer()->setClearColor(Color::Red());
		//getRenderer()->render( *getScene() );
		getRenderer()->render( *getScene(), &mRenderTarget);

		//getRenderer()->setClearColor(Color::green());
		

		//mOffscreenTexture.dump("test3.jpg");

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		mRenderTarget.unbind();

		////// TODO fix this
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Texture2D tmp;
		//tmp.create();
		//tmp.setData(
		//	getManager()->getWindow()->getWidth(), getManager()->getWindow()->getHeight(),
		//	GL_RGB, GL_RGB, NULL, GL_UNSIGNED_BYTE, true);

		//Texture2D::copy(&mOffscreenTexture, &tmp);
	
		//

		getRenderer()->render(
			&mOffscreenTexture,
			glm::vec2(getManager()->getWindow()->getWidth(), getManager()->getWindow()->getHeight()),
			0, 0,
			getManager()->getWindow()->getWidth(), getManager()->getWindow()->getHeight()
		);


		//mOffscreenTexture.dump("offscreen.bmp");
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

		getAssets()->load("level2.lua");


		mDepthBuffer.create();
		mDepthBuffer.setStorage(GL_DEPTH_COMPONENT,
			getManager()->getWindow()->getWidth(), getManager()->getWindow()->getHeight());

		mOffscreenTexture.create();

		
		

		mOffscreenTexture.setData(
			getManager()->getWindow()->getWidth(), getManager()->getWindow()->getHeight(),
			GL_RGB, GL_RGB, NULL, GL_UNSIGNED_BYTE, true);

		mRenderTarget.getFrameBuffer().create();

		mRenderTarget.getFrameBuffer().addAttachment(GL_COLOR_ATTACHMENT0, &mOffscreenTexture);
		mRenderTarget.getFrameBuffer().addAttachment(GL_DEPTH_ATTACHMENT, mDepthBuffer);

		mRenderTarget.addBuffer(GL_COLOR_ATTACHMENT0);

		WT_ASSERT(mRenderTarget.getFrameBuffer().isComplete(), "Incomplete framebuffer");

		SceneLoader loader(getScene(), getAssets());

		loader.load("scene-test.lua");
	}

	String getConfigFile() const{
		return "assets/OffscreenDemoConfig.lua";
	}

	//String getLevelFile() const{
	//	//return "world.lua";
	//}

}; // </OffscreenDemo>

WT_DECLARE_DEMO(OffscreenDemo)

}; // </wt>

#endif // </WT_OFFSCREENDEMO_H>
