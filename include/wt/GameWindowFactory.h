#ifndef WT_GAMEWINDOWFACTORY_H
#define WT_GAMEWINDOWFACTORY_H

#include "wt/AGameWindow.h"

namespace wt
{

class GameWindowFactory{
public:

	static AGameWindow* create(const AGameWindow::VideoMode& desc);

}; // </GameWindowFactory>

} // </wt>

#endif // </WT_GAMEWINDOWFACTORY_H>