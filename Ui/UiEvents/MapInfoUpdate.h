#ifndef MAPINFOUPDATE
#define MAPINFOUPDATE

class MapInfoUpdate
{
	public:
		MapInfoUpdate(int num, int seaLvl, int hH, int lH, int perc)
		{
			mapNum = num;
			seaLevel = seaLvl;
			highestH = hH;
			lowestH = lH;
			percentComplete = perc;
		}

		int mapNum;
		int seaLevel;

		int highestH;
		int lowestH;

		int percentComplete;
};

#endif
