#ifndef MAP
#define MAP

#include "Pos.h"
#include "MapTile.h"

#define MAX_H 12		//170//120//70

#define INIT_H ((MAX_H / 2) - 1)

#define SEA_LEVEL (MAX_H / 2)

class Map
{
	private:
		int mapHeight, mapWidth;
		MapTile **map;

		int highestH, lowestH;

	public:
		Map(int w, int h);
		~Map();

		MapTile& Tile(Pos p);
		MapTile& Tile(int x, int y);

		void normalize(int maxH);

		bool isPosInsideWrap(Pos p);
		bool isPosInsideWrap(int x, int y);

		bool isPosInsideNoWrap(Pos p);
		bool isPosInsideNoWrap(int x, int y);

		int getMapHeight();
		int getMapWidth();

		void setHighestH(int h);
		void setLowestH(int h);

		int getHighestH();
		int getLowestH();

};

#endif