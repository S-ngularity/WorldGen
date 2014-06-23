#ifndef MAP
#define MAP

#define MAPHEIGHT 270
#define MAPWIDTH 480

#define MULTIPLIER 1

#define MAX_H 70

#define INIT_H ((MAX_H / 2) - 1)

#define SEA (MAX_H / 2)

class Pos
{
	private:
		int x;
		int y;

	public:
		Pos();
		Pos(int initX, int initY);

		void setPos(int newX, int newY);
		int getX();
		int getY();
};


class MapTile
{
	private:
		//Pos pos;
		int h;
		int chance;
		Pos pred;
		bool isSeed;
		bool skip;
		bool error;

		//void setPos(int x, int y);

	public:
		bool seedLow;

		MapTile();

		int getH();
		void setH(int newH);

		int getChance();
		void setBaseChance(); // set chance de próximos tiles manterem altura
		void lowerChance(MapTile prevTile); // diminui a chance da nova posição baseada na chance da posição anterior, retorna nova chance

		Pos getPred();
		void setPred(Pos newPred);
		void setPred(int predX, int predY);

		bool getIsSeed();
		void setIsSeed(bool newIsSeed);

		bool getSkip();
		void setSkip(bool newSkip);

		bool getError();
		void setError(bool newError);
};


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