#ifndef WALKWINDOWOPENED
#define WALKWINDOWOPENED

#include "map.h"

class WalkWindowOpened
{
	public:
		WalkWindowOpened(Map *m, int a, int b)
		{
			map = m;
			x = a;
			y = b;
		}

		Map *map;
		int x, y;

};

#endif