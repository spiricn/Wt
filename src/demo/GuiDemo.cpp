#include "demo/stdafx.h"

#include <wt/Toast.h>
#include <wt/Sp.h>

#include "demo/ADemo.h"
#include "wt/EventTable.h"
#include "wt/gui/SliderView.h"
#include "wt/gui/CircleView.h"
#include "wt/MemberCallback.h"
#include "wt/gui/ListView.h"

#define TD_TRACE_TAG "GuiDemo"

using namespace wt;

class GuiDemo : public ADemo{
public:
	GuiDemo() : mProgress(0.0f){
	}

	void onUpdate(float dt){
		getCameraControl()->handle(dt);

		if(mUi->findView<gui::Checkbox>("cb")->isChecked()){
			// Automatic camera rotation
			mProgress = fmod(mProgress + 10*dt, 100.0f);

			getScene()->getCamera().setRotation( glm::angleAxis( (mProgress/100.0f)*360.0f, glm::vec3(0, 1, 0)) );

			((gui::ProgressView*)mUi->findView("progress_bar"))->setProgress(mProgress );
		}
	}

	void onDemoStart(const LuaObject& config){
		gui::Window* window = (*getScene()->getWindowManager()->getWindowsBeg());

		mUi = window;

		// Window setup
		window->setSize(glm::vec2(900, 600));
		window->setPosition(glm::vec2(100, 100));

		mUi->setColor(Color(0.2, 0.2, 0.2, 0.7));
		mUi->setGridSize(20, 10);
		mUi->setDefaultScaleMode(gui::View::eSCALE_MODE_GRID);
		
		{
			// Button used to change text color
			gui::Button* v =  mUi->createView<gui::Button>();
			v->setGridLocation(2, 2, 1, 2);
			v->setText("Change color");

			getEventManager()->registerCallback(this, &GuiDemo::onBtnClicked, 
				gui::ButtonClickedEvent::TYPE, v);
		}
		
		{
			// Camera rotation progress
			gui::ProgressView* v = mUi->createView<gui::ProgressView>("progress_bar");

			v->setGridLocation(10, 2, 1, 2);
			v->setDrawProgress(true);
			v->setProgress(50);
		}

		{
			// Click counter button
			gui::Button* v = mUi->createView<gui::Button>("btn_clicks");

			v->setGridLocation(4, 2, 1, 2);
			v->setText("Clicks 0");
			v->setBackgroundColor(Color(1, 1, 1, 0.5));

			getEventManager()->registerCallback(this, &GuiDemo::onBtnClicksClicked, 
				gui::ButtonClickedEvent::TYPE, v);
		}

		{
			// Demo stopping button
			gui::Button* v = mUi->createView<gui::Button>();
			v->setGridLocation(6, 2, 1, 2);
			v->setText("Stop demo");

			getEventManager()->registerCallback(this, &GuiDemo::onBtnStopClicked, 
				gui::ButtonClickedEvent::TYPE, v);
		}

		{
			// Toast button
			gui::Button* v = mUi->createView<gui::Button>();
			v->setGridLocation(8, 2, 1, 2);
			v->setText("Toast !");

			getEventManager()->registerCallback(this, &GuiDemo::onToast, 
				gui::ButtonClickedEvent::TYPE, v);
		}

		{
			// Text view
			gui::TextView* v = mUi->createView<gui::TextView>("text");
			v->setGridLocation(3, 5, 12, 4);
			v->setBackgroundColor(Color::Gray());
			v->setTextColor(Color::Black());
			v->setText( kTEST_TEXT );
			
		}

		{
			// Text color view
			gui::TextView* v = mUi->createView<gui::TextView>("clr_text");
			v->setGridLocation(2, 5, 1, 4);
			v->setTextScaleMode(gui::TextView::eSCALE_AUTO);
			v->setBackgroundColor(Color::White());
			v->setText("Current text color: default");
			v->setTextColor(Color::Black());
			v->setPositionFlags(gui::TextView::ePOS_CENTER_HORIZONTAL | gui::TextView::ePOS_CENTER_VERTICAL );
		}

		{
			// Camera rotation slider
			gui::SliderView* v = mUi->createView<gui::SliderView>("slider");
			
			v->setGridLocation(12, 2, 1, 2);

			getEventManager()->registerCallback(this, &GuiDemo::onSliderChanged, 
				gui::SliderValueChangedEvent::TYPE, v);
		}

		{
			// Automatic rotation toggle checkbox
			gui::Checkbox* v = mUi->createView<gui::Checkbox>("cb");
			v->setText("Rotation");
			v->setGridLocation(14, 2, 1, 2);
			v->setChecked(true);
		}

		{
			// Manual camera slider
			gui::SliderView* v = mUi->createView<gui::SliderView>("camera_slider");
			
			v->setGridLocation(16, 2, 1, 2);

			getEventManager()->registerCallback(this, &GuiDemo::rotateCamera, 
				gui::SliderValueChangedEvent::TYPE, v);
		}

		{
			// Simple color list
			gui::ListView* v = mUi->createView<gui::ListView>("list");
			
			v->addItem("1) Red", reinterpret_cast<void*>(1));
			v->addItem("2) Green", reinterpret_cast<void*>(2));
			v->addItem("3) Blue", reinterpret_cast<void*>(3));

			v->setGridLocation(18, 2, 1, 2);

			getEventManager()->registerCallback(this, &GuiDemo::onListItemChanged,
				gui::ListItemSelectedEvent::TYPE, v);
		}

		// Let the user control the mouse
		getInput()->setMouseGrabbed(false);
	}

	void onListItemChanged(){
		gui::ListView* v = mUi->findView<gui::ListView>("list");

		LOGI("Item changed %s", v->getCurrentItem()->name.c_str());

		switch(reinterpret_cast<int>(v->getCurrentItem()->userData)){
		case 1:
			((gui::TextView*)mUi->findView("text"))->setTextColor(Color::Red());
			break;
		case 2:
			((gui::TextView*)mUi->findView("text"))->setTextColor(Color::Green());
			break;
		case 3:
			((gui::TextView*)mUi->findView("text"))->setTextColor(Color::Blue());
			break;
		}
	}

	void onSliderChanged(){
		float val = mUi->findView<gui::SliderView>("slider")->getValue() / 100.0f;

		mUi->findView<gui::TextView>("text")->setFontScale( glm::mix(0.5f, 2.0f, val ) );
	}

	void onToast(){
		getProcessManager()->attach(
			(new Toast(mUi, glm::vec2(getWindow()->getWidth()/2, getWindow()->getHeight()/2), " Toast message test ! "))
			->setDuration(1)
			->setFadeInTime(0.3)
			->setFadeOutTime(0.3)
			->setFadeInValue(1.0f)
			);
	}

	void onBtnStopClicked(){
		stopMainLoop();
	}

	void onBtnClicksClicked(){
		// Click counter
		static uint32_t numClicks = 0;

		mUi->findView<gui::Button>("btn_clicks")->setText( utils::print("Clicks %d", ++numClicks) );
	}

	void setColor(const Color& clr){
		mUi->findView<gui::TextView>("clr_text")->setText( utils::print("Current text color: %s", clr.hex()) );

		mUi->findView<gui::TextView>("text")->setTextColor(clr);
	}

	void rotateCamera(){
		float val = mUi->findView<gui::SliderView>("camera_slider")->getValue()/100.0f;
		
		getScene()->getCamera().setRotation( glm::angleAxis(val*360.0f, glm::vec3(0, 1, 0)) );
	}

	void onBtnClicked(){
		// Set a random color
		setColor( Color::random() );
	}

	String getScriptPath() const{
		return "assets/GuiDemoConfig.lua";
	}

private:
	float mProgress;
	gui::Layout* mUi;
	static const char* kTEST_TEXT;

}; // </GuiDemo>

const char* GuiDemo::kTEST_TEXT = 
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

WT_DECLARE_DEMO_IMPL(GuiDemo)
