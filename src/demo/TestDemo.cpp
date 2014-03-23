#include "demo/stdafx.h"

#include "demo/ADemo.h"

#include <wt/gui/ListView.h>

#include <wt/MusicPlayer.h>
#include <wt/OwnPtr.h>

using namespace wt;

#define TD_TRACE_TAG "TestDemo"

class TestDemo : public ADemo{
public:
	TestDemo(){
	}

	void onUpdate(float dt){
		getCameraControl()->handle(dt, getInput());
	}
	
	void onKeyUp(wt::VirtualKey code){
		ADemo::onKeyUp(code);
	}
	
	void onDemoStart(const LuaObject& config){
		mUi = getScene()->getUIWindow();

		gui::ListView* view = mUi->createView<gui::ListView>();

		for(int i=0; i<10; i++){

			char name[64];
			sprintf(name, "Test item %d", i);

			view->addItem(name);
		}


		/*gui::SliderView* sli = mUi->createView<gui::SliderView>();

		sli->setSize(30, 200);

		sli->setPosition(300, 300);
		
		sli->setMode(gui::SliderView::eMODE_VERTICAL);*/

	}

	//String getScriptPath() const{
		//return "demo_workspace/TestDemo/main.lua";
	//}

	gui::Window* mUi;
};

WT_DECLARE_DEMO_IMPL(TestDemo);
