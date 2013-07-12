#ifndef WT_GUIDEMO_H
#define WT_GUIDEMO_H

#include "demo/ADemo.h"
#include "wt/EventTable.h"
#include "wt/gui/SliderView.h"
#include "wt/gui/CircleView.h"
#include <wt/Toast.h>
#include <wt/Sp.h>

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

namespace wt{

class GuiDemo : public ADemo{
private:

	float mProgress;
	bool mProgressBarPaused;

public:

	GuiDemo() : mProgress(0.0f), mProgressBarPaused(false){
	}

	void onRender(float dt){
		getRenderer()->render( *getScene() );
	}

	void onMouseMotion(const MouseMotionEvent* evt){
	}


	void onUpdate(float dt){
		getPhysics()->update(dt);
		getScene()->update(dt);
		getCameraControl()->handle(dt, getManager()->getInput());


		if(!mProgressBarPaused){
			if(math::random() > 0.9){
				mProgress = fmod(mProgress + math::random()*100*dt, 100.0f);
			}
		
			((Gui::ProgressView*)getUi().findView("PROGRESS_VIEW"))->setProgress(mProgress );
		}
	}

	void onKeyDown(VirtualKey c){
		ADemo::onKeyDown(c);
	}

	void onStart(const LuaObject& config){
		getCameraControl()->setCamera(&getScene()->getCamera());


		Gui::Button* btn;
		
		getUi().setGridSize(20, 10);
		getUi().setDefaultScaleMode(Gui::View::eGRID);
		
		{
			btn = getUi().createView<Gui::Button>();
			btn->setGridLocation(2, 2, 1, 2);
			btn->setText("Change color");
			

			getEventManager()->registerCallback(
				new MemberCallback<GuiDemo>(this, &GuiDemo::onBtnClicked), Gui::ButtonClickedEvent::TYPE, true, btn->getId()
				);
		}

		{
			
			btn = getUi().createView<Gui::Button>("");

			btn->setGridLocation(4, 2, 1, 2);
			btn->setText("Nudge");

			getEventManager()->registerCallback(
				new MemberCallback<GuiDemo>(this, &GuiDemo::onBtnNudgeClicked), Gui::ButtonClickedEvent::TYPE, true, btn->getId()
				);
		}

		{
			btn = getUi().createView<Gui::Button>();
			btn->setGridLocation(6, 2, 1, 2);
			btn->setText("Stop demo");

			getEventManager()->registerCallback(
				new MemberCallback<GuiDemo>(this, &GuiDemo::onBtnStopClicked), Gui::ButtonClickedEvent::TYPE, true, btn->getId()
				);
		}

		{
			btn = getUi().createView<Gui::Button>();
			btn->setGridLocation(8, 2, 1, 2);
			btn->setText("Toast !");

			getEventManager()->registerCallback(
				new MemberCallback<GuiDemo>(this, &GuiDemo::onToast), Gui::ButtonClickedEvent::TYPE, true, btn->getId()
				);
		}

		{
			Gui::ProgressView* v = getUi().createView<Gui::ProgressView>("PROGRESS_VIEW");

			v->setGridLocation(10, 2, 1, 2);
			v->setDrawProgress(true);
			v->setSize( btn->getSize() + glm::vec2(0, 10) );
		}
		{
			
			Gui::TextView* v = getUi().createView<Gui::TextView>("text");
			v->setGridLocation(3, 5, 12, 4);
			v->setBackgroundColor(Color::gray());
			v->setTextColor(Color::black());
			v->setText( kTEST_TEXT );
			
		}

		{
			
			Gui::TextView* v = getUi().createView<Gui::TextView>("clr_text");
			v->setGridLocation(2, 5, 1, 4);
			v->setScalingMode(Gui::TextView::eAUTO);
			v->setBackgroundColor(Color::white());
			v->setText("Current text color: default");
			v->setTextColor(Color::black());
		}
		{
			
			Gui::SliderView* v = getUi().createView<Gui::SliderView>("slider");
			
			v->setGridLocation(12, 2, 1, 2);

			getEventManager()->registerCallback(
				new MemberCallback<GuiDemo>(this, &GuiDemo::onSliderChanged), Gui::SliderValueChangedEvent::TYPE, true, v->getId()
				);
		}

		{
			Gui::Checkbox* v = getUi().createView<Gui::Checkbox>("cb");
			v->setText("Progress bar paused");
			v->setGridLocation(14, 2, 1, 2);

			getEventManager()->registerCallback(
				new MemberCallback<GuiDemo>(this, &GuiDemo::onCheckboxClicked), Gui::CheckboxClickedEvent::TYPE, true, v->getId()
				);
		}
		getRenderer()->setClearColor( Color::black() );

		getManager()->getInput()->setMouseGrabbed(false);
	}

	void onCheckboxClicked(){
		mProgressBarPaused = !mProgressBarPaused;
	}

	void onSliderChanged(){
		float val = ((Gui::SliderView*)getUi().findView("slider"))->getValue()/100.0f;

		setColor(Color::fromHSV(val, 0.5, 0.5));
	}

	void onToast(){
		getProcManager().attach(
			(new Toast(&getUi(), glm::vec2(500, 100), glm::vec2(500, 500), getAssets()->getTextureManager()->find("toast_test")))
			->setDuration(1)
			->setFadeInTime(0.3)
			->setFadeOutTime(0.3)
			->setFadeInValue(1.0f)
			);
	}

	void onBtnStopClicked(){
		stopDemo();
	}

	void onBtnNudgeClicked(){
		Gui::View* v = getUi().findView("text");
		
		v->setPosition(v->getPosition() + glm::vec2(-5 + math::random()*10, -5 + math::random()*10) );
	}

	void setColor(const Color& clr){
		String t = "Current text color: ";
		t.append(clr.hex());

		((Gui::TextView*)getUi().findView("clr_text"))->setText(t);
		((Gui::TextView*)getUi().findView("text"))->setTextColor(clr);
	}

	void onBtnClicked(){	
		setColor(Color::random());
	}

	String getConfigFile() const{
		return "";
	}

	String getLevelFile() const{
		return "assets/demo/GuiDemo/level.lua";
	}

}; // </GuiDemo>

WT_DECLARE_DEMO(GuiDemo)

}; // </wt>

#endif // </WT_GUIDEMO_H>
