#ifndef MAP
#define MAP

#define MAPSIZE 120

#define MULTIPLIER 1

#define MAX_H 7

class Pos
{
	private:

	public:
		int x;
		int y;

		Pos();
		Pos(int initX, int initY);

		void setPos(int newX, int newY);
};

class Tile
{
	private:

	public:
		Pos pos;
		int h;
		int chance;
		Pos pred;
		bool isSeed;
		bool skip;
		int printPred;
		int visitado;

		Tile();
		Tile(int x, int y);
		Tile(Pos p);
};

class Map
{
	private:
		Tile map[MAPSIZE][MAPSIZE];

	public:
		Tile getTile(Pos p) throw(bool);
		Tile getTile(int x, int y) throw(bool);
		void setTile(Tile t);

		// funções de inserção de seeds -- retornam posição da seed inserida
		Pos insertSeed();

		//set chance de próximos tiles manterem altura
		void setBaseChance(Pos p);
		// diminui a chance da nova posição baseada na chance da posição anterior, retorna nova chance
		int lowerChance(Pos oldP);

		bool isInside(Pos p);
};

#endif