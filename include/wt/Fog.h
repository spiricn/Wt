#ifndef WT_FOG_H
#define WT_FOG_H

#include "wt/Color.h"

namespace wt
{

struct FogDesc{
	Color color;
	float density;
	bool enabled;

	FogDesc() : color(Color::Gray()), density(0.008f), enabled(true){
	}

}; // </Fog>

}; // </wt>

#endif // </WT_FOG_H>