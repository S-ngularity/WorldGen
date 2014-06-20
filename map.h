#ifndef MAP
#define MAP

#define MAPSIZE 150

#define MULTIPLIER 1

#define MAX_H 11

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
		MapTile map[MAPSIZE][MAPSIZE];

		// funções de inserção de seeds -- retornam posição da seed inserida
		Pos insertSeedHigh();
		Pos insertSeedLow();

	public:
		//Map();

		bool isPosInside(Pos p);

		MapTile& Tile(Pos p);
		MapTile& Tile(int x, int y);

		// funções de inserção de artefatos
		void insertHighArtifact();
		void insertLowArtifact();
};

#endif