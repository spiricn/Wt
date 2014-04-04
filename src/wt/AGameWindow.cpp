#include "wt/stdafx.h"
#include "wt/AGameWindow.h"
#include "wt/Lua.h"

#define TD_TRACE_TAG "AGameWindow"

namespace wt
{

using namespace lua;

AGameWindow::~AGameWindow(){
}

uint32_t AGameWindow::getWidth() const{
	return getDesc().screenWidth;
}

uint32_t AGameWindow::getHeight() const{
	return getDesc().screenHeight;
}

float AGameWindow::getAspectRatio() const{
	return static_cast<float>(getDesc().screenWidth)/getDesc().screenHeight;
}

void AGameWindow::Desc::serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const{
	dst.Set("screen_width", screenWidth);
	dst.Set("screen_height", screenHeight);

	dst.Set("screen_posX", screenPosX);
	dst.Set("screen_posY", screenPosY);

	dst.Set("pixel_depth", pixelDepth);
	dst.Set("window_title", windowTitle.c_str());

	dst.Set("vsync_enabled", vsyncEnabled);

	String modeStr;

	switch(mode){
	case eMODE_FULLSCREEN:
		modeStr = "eMODE_FULLSCREEN";
		break;

	case eMODE_WINDOWED:
		modeStr = "eMODE_WINDOWED";
		break;

	case eMODE_WINDOWED_BORDERLESS:
		modeStr = "eMODE_WINDOWED_BORDERLESS";
		break;

	default:
		WT_THROW("Unsupported window mode");
	}

	dst.Set("mode", modeStr.c_str());
}

void AGameWindow::Desc::deserialize(lua::State* luaState, const LuaPlus::LuaObject& src){
	luaConv(src.Get("screen_width"), screenWidth);
	luaConv(src.Get("screen_height"), screenHeight);

	luaConv(src.Get("screen_posX"), screenPosX);
	luaConv(src.Get("screen_posY"), screenPosY);

	luaConv(src.Get("pixel_depth"), pixelDepth);
	luaConv(src.Get("window_title"), windowTitle);
	luaConv(src.Get("vsync_enabled"), vsyncEnabled);

	String modeStr;
	if(luaConv(src.Get("mode"), modeStr)){
		if(modeStr.compare("eMODE_FULLSCREEN") == 0){
			mode = eMODE_FULLSCREEN;
		}
		else if(modeStr.compare("eMODE_WINDOWED") == 0){
			mode = eMODE_WINDOWED;
		}
		else if(modeStr.compare("eMODE_WINDOWED_BORDERLESS") == 0){
			mode = eMODE_WINDOWED_BORDERLESS;
		}
		else{
			WT_THROW("Unsupported window mode");
		}
	}
}

AGameWindow::Desc::Desc(uint32_t screenWidth, uint32_t screenHeight, const String& windowTitle,
		bool vsyncEnabled, uint8_t pixelDepth, Mode mode) : screenWidth(screenWidth), screenHeight(screenHeight), windowTitle(windowTitle), 
			vsyncEnabled(vsyncEnabled), pixelDepth(pixelDepth){
}



const EventType WindowSizeChange::TYPE = "WindowSizeChange";

} // </wt>
