#ifndef WALKWINDOWOPENED
#define WALKWINDOWOPENED

#include "Map/Map.h"

class WalkWindowOpened
{
	public:
		WalkWindowOpened(const Map &m, int a, int b) : 
			map(m), 
			x(a), 
			y(b)
		{}

		const Map &map;
		int x, y;

};

#endif
