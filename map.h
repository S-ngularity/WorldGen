#ifndef MAP
#define MAP

#define MAPSIZE 250

#define MULTIPLIER 1

#define MAX_H 25

#define NULO -1

// struct de posição
typedef struct pos
{
	int x;
	int y;
} Pos;

// struct de cada tile do mapa
typedef struct tile
{
	int h;
	int chance;
	Pos pred;
	int isSeed;
	int skip;
	int printPred;
	int visitado;
} Tile;

// funções de inserção de seeds
Pos insertSeed(int &h, Tile m[MAPSIZE][MAPSIZE]);

//set chance de próximos tiles manterem altura
int setBaseChance(Tile m[MAPSIZE][MAPSIZE], int x, int y); // retorna chance base do tile de acordo com altura dele
int lowerChance(Tile m[MAPSIZE][MAPSIZE], int oldX, int oldY); // retorna nova chance diminuida baseada na altura e chance de outro tile (anterior)

//imprimir tile na cor certa (no Windows)
void printMap(Tile m[MAPSIZE][MAPSIZE]);
void printMapWithPreds(Tile m[MAPSIZE][MAPSIZE]);
void printColor(int n);
void printSeed(int n);
void printPred(int n);

#endif