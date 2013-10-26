#ifndef WT_GAMEINPUTFACTORY_H
#define WT_GAMEINPUTFACTORY_H

#include "wt/AGameInput.h"

namespace wt
{

class GameInputFactory{
public:

	static AGameInput* create();

}; // </GameWindowFactory>

} // </wt>

#endif // </WT_GAMEINPUTFACTORY_H>