#ifndef WT_CONSOLE_H
#define WT_CONSOLE_H


#include "wt/stdafx.h"

#include "wt/Singleton.h"

namespace wt{

class Console : public Singleton<Console>{
public:
	enum ConsoleColor{
		eBLACK = 0x0 ,
		eRED = 0x4,
		eGREEN = 0x2,
		eBLUE = 0x1,
		eCYAN = eGREEN | eBLUE,
		eMAGENTA = eBLUE | eRED,
		eYELLOW = eRED | eGREEN,
		eWHITE = eRED | eGREEN | eBLUE,
		eINTENSE = 0x8,
	};

	static void setColor(ConsoleColor text=eWHITE, ConsoleColor background=eBLACK);

	static void setPosition(int32_t x, int32_t y, int32_t w, int32_t h);

	static void cpritnf(ConsoleColor textColor, ConsoleColor backgroundColor, const char* fmt, ...);


#if defined(WIN32)
private:
	static int convertColor(ConsoleColor color);
#endif

}; // </Console>

inline Console::ConsoleColor operator|(Console::ConsoleColor  a, Console::ConsoleColor  b){
		return static_cast<Console::ConsoleColor>( static_cast<int>(a) | static_cast<int>(b) );
}

}; // </wt>

#define TheConsole wt::Console::getSingleton()

#endif
