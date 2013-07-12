#include "wt/stdafx.h"

#include "wt/Console.h"

#define LOG_BFR_SIZE 2048

#define TD_TRACE_TAG "Console"

namespace wt{

void Console::setColor(ConsoleColor text, ConsoleColor background){
#if defined(WIN32)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
			( convertColor(background)<<4) | convertColor(text));
#else
	#error Not implemented on this platform
#endif
}


void Console::setPosition(Int32 x, Int32 y, Int32 w, Int32 h){
#if defined(WIN32)
	HWND wnd = GetConsoleWindow();
	MoveWindow(wnd, x, y, w, h, 1);
#else
	#error Not implemented on this platform
#endif
}

#if defined(WIN32)
int Console::convertColor(ConsoleColor color){
	// TODO do this properly
	return static_cast<int>(color);
}
#endif


void Console::cpritnf(ConsoleColor textColor, ConsoleColor backgroundColor, const char* fmt, ...){
	static char bfr[LOG_BFR_SIZE];
	bfr[0] = 0;

	va_list argList;
	va_start(argList, fmt);

	vsnprintf(bfr, LOG_BFR_SIZE, fmt, argList);

	setColor(textColor, backgroundColor);

	fputs(bfr, stdout);
}


}; // </wt>