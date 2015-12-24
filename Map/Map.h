#ifndef MAP
#define MAP

#include <vector>

#define MAX_H 255 //170//120//70

#define INIT_H ((MAX_H / 2) - 1)

#define SEA_LEVEL ((MAX_H / 2) - 1)

class Map
{
	private:
		int mapWidth, mapHeight;
		std::vector<int> map;

		int highestH, lowestH;
		int seaLevel;

	public:
		Map(int w, int h);
		Map(const Map &) = default;
		Map& operator=(const Map&) = default;
		~Map() = default;

		int getH(int x, int y);
		void setH(int x, int y, int newH);

		void normalize(int maxH);

		bool isPosInsideWrap(int x, int y);

		bool isPosInsideNoWrap(int x, int y);

		int getMapHeight();
		int getMapWidth();

		int getHighestH();
		void setHighestH(int h);

		int getLowestH();
		void setLowestH(int h);

		int getSeaLevel();
		void setSeaLevel(int lvl);
		void increaseSeaLevel();
		void decreaseSeaLevel();
};

#endif
