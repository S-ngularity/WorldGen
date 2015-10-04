#ifndef MAPINFOUPDATE
#define MAPINFOUPDATE

#include <string>

class MapInfoUpdate
{
	public:
		MapInfoUpdate(std::string nName, int num, int seaLvl, int hH, int lH, int perc) : 
			noiseName(nName)
		{
			mapNum = num;
			seaLevel = seaLvl;
			highestH = hH;
			lowestH = lH;
			percentComplete = perc;
		}

		std::string noiseName;

		int mapNum;
		int seaLevel;

		int highestH;
		int lowestH;

		int percentComplete;
};

#endif
