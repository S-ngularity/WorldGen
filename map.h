#ifndef MAP
#define MAP

#include "Pos.h"
#include "MapTile.h"

#define MAPHEIGHT 768	//768//540//270
#define MAPWIDTH 1366	//1366//960//480

#define MAX_H 170		//150//120//70

#define INIT_H ((MAX_H / 2) - 1)

#define SEA (MAX_H / 2)

class Map
{
	private:
		MapTile **map;

		// funções de inserção de seeds -- retornam posição da seed inserida
		Pos insertSeedHigh(Pos seedPos, float highMultiplier);
		Pos insertSeedLow(Pos seedPos, float lowMultiplier);

	public:
		Map();
		~Map();

		MapTile& Tile(Pos p);
		MapTile& Tile(int x, int y);

		bool isPosInsideNoWrap(Pos p);
		bool isPosInsideWrap(Pos p);

		// funções de inserção de artefatos
		void insertHighArtifact(Pos seedPos, int deltaH);
		void insertLowArtifact(Pos seedPos, int deltaH);
};

#endif