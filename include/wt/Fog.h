#ifndef WT_FOG_H
#define WT_FOG_H

#include "wt/Color.h"

namespace wt{

struct Fog{
	Color color;
	float density;

	Fog() : color(Color::Gray()), density(0.008f){
	}

}; // </Fog>

}; // </wt>

#endif // </WT_FOG_H>