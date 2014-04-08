#include "demo/stdafx.h"

#include <wt/Toast.h>
#include <wt/Sp.h>

#include "demo/ADemo.h"
#include "wt/EventTable.h"
#include "wt/gui/SliderView.h"
#include "wt/gui/CircleView.h"
#include "wt/MemberCallback.h"

#define TD_TRACE_TAG "GuiDemo"

static const char* kTEST_TEXT = 
	"Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod\n"
	"tincidunt ut laoreet dolore magna aliquam erat volutpat.\n"
	"Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat.\n"
	"Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat,\n"
	"vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim\n"
	"qui blandit praesent luptatum zzril delenit augue duis dolore\n"
	"te feugait nulla facilisi. Nam liber tempor cum soluta nobis eleifend option\n"
	"congue nihil imperdiet doming id quod mazim placerat facer possim assum.\n"
	"Typi non habent claritatem insitam; est usus legentis in iis qui facit eorum claritatem.\n"
	"Investigationes demonstraverunt lectores legere me lius quod ii legunt saepius.\n"
	"Claritas est etiam processus dynamicus, qui sequitur mutationem consuetudium lectorum.\n"
	"Mirum est notare quam littera gothica, quam nunc putamus parum claram, anteposuerit litterarum formas\n"
	"humanitatis per seacula quarta decima et quinta decima. Eodem modo typi, qui nunc nobis videntur parum clari, fiant sollemnes in futurum.";

using namespace wt;


class GuiDemo : public ADemo{
public:

	GuiDemo() : mProgress(0.0f), mProgressBarPaused(true){
	}

	void onRender(float dt){
		getRenderer()->render( *getScene() );
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}

	void onMouseMotion(const MouseMotionEvent* evt){
	}


	void onUpdate(float dt){
		getCameraControl()->handle(dt);

		if(!mProgressBarPaused){
			mProgress = fmod(mProgress + 10*dt, 100.0f);

			getScene()->getCamera().setRotation( glm::angleAxis( (mProgress/100.0f)*360.0f, glm::vec3(0, 1, 0)) );

			((gui::ProgressView*)mUi->findView("PROGRESS_VIEW"))->setProgress(mProgress );
		}
	}

	void onKeyDown(VirtualKey c){
		ADemo::onKeyDown(c);
	}

	void onDemoStart(const LuaObject& config){
		getCameraControl()->setCamera(&getScene()->getCamera());

		gui::Button* btn;

		// TODO a better way of getting the default window?
		mUi = *getScene()->getWindowManager()->getWindowsBeg();
		
		mUi->setGridSize(20, 10);
		mUi->setDefaultScaleMode(gui::View::eSCALE_MODE_GRID);
		
		{
			btn = mUi->createView<gui::Button>();
			btn->setGridLocation(2, 2, 1, 2);
			btn->setText("Change color");

			getEventManager()->registerCallback(this, &GuiDemo::onBtnClicked, 
				gui::ButtonClickedEvent::TYPE, btn);
		}

		{
			
			btn = mUi->createView<gui::Button>("");

			btn->setGridLocation(4, 2, 1, 2);
			btn->setText("Nudge");

			getEventManager()->registerCallback(this, &GuiDemo::onBtnNudgeClicked, 
				gui::ButtonClickedEvent::TYPE, btn);
		}

		{
			btn = mUi->createView<gui::Button>();
			btn->setGridLocation(6, 2, 1, 2);
			btn->setText("Stop demo");

			getEventManager()->registerCallback(this, &GuiDemo::onBtnStopClicked, 
				gui::ButtonClickedEvent::TYPE, btn);
		}

		{
			btn = mUi->createView<gui::Button>();
			btn->setGridLocation(8, 2, 1, 2);
			btn->setText("Toast !");

			getEventManager()->registerCallback(this, &GuiDemo::onToast, 
				gui::ButtonClickedEvent::TYPE, btn);
		}

		{
			gui::ProgressView* v = mUi->createView<gui::ProgressView>("PROGRESS_VIEW");

			v->setGridLocation(10, 2, 1, 2);
			v->setDrawProgress(true);
			v->setSize( btn->getSize() + glm::vec2(0, 10) );
		}
		{
			
			gui::TextView* v = mUi->createView<gui::TextView>("text");
			v->setGridLocation(3, 5, 12, 4);
			v->setBackgroundColor(Color::Gray());
			v->setTextColor(Color::Black());
			v->setText( kTEST_TEXT );
			
		}

		{
			
			gui::TextView* v = mUi->createView<gui::TextView>("clr_text");
			v->setGridLocation(2, 5, 1, 4);
			v->setScalingMode(gui::TextView::eSCALE_AUTO);
			v->setBackgroundColor(Color::White());
			v->setText("Current text color: default");
			v->setTextColor(Color::Black());
		}
		{
			
			gui::SliderView* v = mUi->createView<gui::SliderView>("slider");
			
			v->setGridLocation(12, 2, 1, 2);

			getEventManager()->registerCallback(this, &GuiDemo::onSliderChanged, 
				gui::SliderValueChangedEvent::TYPE, v);
		}

		{
			gui::Checkbox* v = mUi->createView<gui::Checkbox>("cb");
			v->setText("Camera rotation paused");
			v->setGridLocation(14, 2, 1, 2);

			getEventManager()->registerCallback(this, &GuiDemo::onCheckboxClicked, 
				gui::CheckboxClickedEvent::TYPE, v);
		}

		{
			
			gui::SliderView* v = mUi->createView<gui::SliderView>("camera_slider");
			
			v->setGridLocation(16, 2, 1, 2);

			getEventManager()->registerCallback(this, &GuiDemo::rotateCamera, 
				gui::SliderValueChangedEvent::TYPE, v);
		}

		getRenderer()->setClearColor( Color::Black() );

		getInput()->setMouseGrabbed(false);
	}

	void onCheckboxClicked(){
		mProgressBarPaused = !mProgressBarPaused;
	}

	void onSliderChanged(){
		float val = ((gui::SliderView*)mUi->findView("slider"))->getValue()/100.0f;

		((gui::TextView*)mUi->findView("text"))->setFontScale( glm::mix(0.5f, 2.0f, val ) );
	}

	void onToast(){
		getProcessManager()->attach(
			(new Toast(mUi, glm::vec2(500, 100), glm::vec2(500, 500), getAssets()->getTextureManager()->find("toasty")))
			->setDuration(1)
			->setFadeInTime(0.3)
			->setFadeOutTime(0.3)
			->setFadeInValue(1.0f)
			);
	}

	void onBtnStopClicked(){
		stopMainLoop();
	}

	void onBtnNudgeClicked(){
		gui::View* v = mUi->findView("text");
		
		v->setPosition(v->getPosition() + glm::vec2(-5 + math::random()*10, -5 + math::random()*10) );
	}

	void setColor(const Color& clr){
		String t = "Current text color: ";
		t.append(clr.hex());

		((gui::TextView*)mUi->findView("clr_text"))->setText(t);
		((gui::TextView*)mUi->findView("text"))->setTextColor(clr);
	}

	void rotateCamera(){
		float val = dynamic_cast<gui::SliderView*>(mUi->findView("camera_slider"))->getValue()/100.0f;

		
		getScene()->getCamera().setRotation( glm::angleAxis(val*360.0f, glm::vec3(0, 1, 0)) );
	}

	void onBtnClicked(){	
		setColor(Color::random());
	}

	String getScriptPath() const{
		return "assets/GuiDemoConfig.lua";
	}

private:
	float mProgress;
	bool mProgressBarPaused;
	gui::Window* mUi;

}; // </GuiDemo>

WT_DECLARE_DEMO_IMPL(GuiDemo)
