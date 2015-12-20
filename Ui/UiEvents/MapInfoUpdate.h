#ifndef MAPINFOUPDATE
#define MAPINFOUPDATE

#include <string>

class MapInfoUpdate
{
	public:
		MapInfoUpdate(int seaLvl, int hH, int lH)
		{
			seaLevel = seaLvl;
			highestH = hH;
			lowestH = lH;
		}

		int seaLevel;

		int highestH;
		int lowestH;
};

#endif
