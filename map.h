#ifndef MAP
#define MAP

#define MAPSIZE 250

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


class Tile
{
	private:
		//Pos pos;
		int h;
		int chance;
		Pos pred;
		bool isSeed;
		bool skip;

		//void setPos(int x, int y);

	public:
		Tile();

		//Pos getPos();
		//int getPosX();
		//int getPosY();

		int getH();
		void setH(int newH);

		int getChance();
		void setBaseChance(); // set chance de próximos tiles manterem altura
		void lowerChance(Tile prevTile); // diminui a chance da nova posição baseada na chance da posição anterior, retorna nova chance

		Pos getPred();
		void setPred(Pos newPred);
		void setPred(int predX, int predY);

		bool getIsSeed();
		void setIsSeed(bool newIsSeed);

		bool getSkip();
		void setSkip(bool newSkip);
};


class Map
{
	private:
		Tile map[MAPSIZE][MAPSIZE];

		// funções de inserção de seeds -- retornam posição da seed inserida
		Pos insertSeedHigh();
		Pos insertSeedLow();

	public:
		//Map();

		bool isPosInside(Pos p);

		Tile& getTile(Pos p);
		Tile& getTile(int x, int y);

		// funções de inserção de artefatos
		void insertHighArtifact();
		void insertLowArtifact();
};

#endif