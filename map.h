#ifndef MAP
#define MAP

#define MAPHEIGHT 200
#define MAPWIDTH 320

#define MULTIPLIER 1

#define MAX_H 35

#define INIT_H MAX_H / 2

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
		bool visited;

		//void setPos(int x, int y);

	public:
		bool seedLow;

		MapTile();

		//Pos getPos();
		//int getPosX();
		//int getPosY();

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

		bool getVisited();
		void setVisited(bool newVisited);
};


class Map
{
	private:
		MapTile map[MAPWIDTH][MAPHEIGHT];

		// funções de inserção de seeds -- retornam posição da seed inserida
		Pos insertSeedHigh(float deltaH);
		Pos insertSeedLow(float deltaH);

	public:
		//Map();

		bool isPosInside(Pos p);

		MapTile& Tile(Pos p);
		MapTile& Tile(int x, int y);

		// funções de inserção de artefatos
		void insertHighArtifact(int deltaH);
		void insertLowArtifact(int deltaH);
};

#endif