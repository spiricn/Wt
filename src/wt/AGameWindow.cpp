#include "wt/stdafx.h"
#include "wt/AGameWindow.h"

#define TD_TRACE_TAG "AGameWindow"

namespace wt
{

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

const EvtType WindowSizeChange::TYPE = "WindowSizeChange";

} // </wt>
