#ifndef WALKWINDOWOPENED
#define WALKWINDOWOPENED

#include "Map/Map.h"

class WalkWindowOpened
{
	public:
		WalkWindowOpened(std::weak_ptr<Map> m, int a, int b)
		{
			map = m;
			x = a;
			y = b;
		}

		std::weak_ptr<Map> map;
		int x, y;

};

#endif
