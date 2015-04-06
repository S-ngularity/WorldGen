#ifndef MAP
#define MAP

#include "Pos.h"
#include "MapTile.h"

#define MAPHEIGHT 1025	//768//540//270
#define MAPWIDTH 1025	//1366//960//480

#define MAX_H 36		//170//120//70

#define INIT_H ((MAX_H / 2) - 1)

#define SEA_LEVEL (MAX_H / 2)

class Map
{
	private:
		MapTile **map;

	public:
		Map();
		~Map();

		MapTile& Tile(Pos p);
		MapTile& Tile(int x, int y);

		bool isPosInsideNoWrap(Pos p);
		bool isPosInsideWrap(Pos p);

		bool isPosInsideNoWrap(int x, int y);
		bool isPosInsideWrap(int x, int y);
};

#endif