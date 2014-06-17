#include <stdio.h>
#include <stdlib.h>
#include "map.h"

#define NULO -1

Pos::Pos()
{
	x = 0;
	y = 0;
}

Pos::Pos(int initX, int initY)
{
	x = initX;
	y = initY;
}

void Pos::setPos(int newX, int newY)
{
	x = newX;
	y = newY;
}

Tile::Tile()
{
	pos.x = 0;
	pos.y = 0;

	h = 0;
	chance = 0;
	pred.x = NULO;
	pred.y = NULO;
	isSeed = false;
	skip = false;
	printPred = 0;
	visitado = 0;
}

Tile::Tile(Pos p)
{
	pos.x = p.x;
	pos.y = p.y;

	h = 0;
	chance = 0;
	pred.x = NULO;
	pred.y = NULO;
	isSeed = false;
	skip = false;
	printPred = 0;
	visitado = 0;
}

Tile::Tile(int x, int y)
{
	pos.x = x;
	pos.y = y;

	h = 0;
	chance = 0;
	pred.x = NULO;
	pred.y = NULO;
	isSeed = false;
	skip = false;
	printPred = 0;
	visitado = 0;
}

Tile Map::getTile(Pos p) throw(bool)
{
	if(p.x < 0 && p.y < 0 && p.x >= MAPSIZE && p.y >= MAPSIZE)
		throw false;
	
	return map[p.x][p.y];
}

Tile Map::getTile(int x, int y) throw(bool)
{
	if(x < 0 && y < 0 && x >= MAPSIZE && y >= MAPSIZE)
		throw false;
	
	return map[x][y];
}

void Map::setTile(Tile t)
{
	map[t.pos.x][t.pos.y] = t;
}

Pos Map::insertSeed()
{
	Pos seedPos;
	int h;

	do{
		seedPos.x = rand() % MAPSIZE;
		seedPos.y = rand() % MAPSIZE;
		h = rand() % MAX_H;
	} while(h < map[seedPos.x][seedPos.y].h);

	map[seedPos.x][seedPos.y].h = h;
	setBaseChance(seedPos);
	map[seedPos.x][seedPos.y].pred.x = NULO;
	map[seedPos.x][seedPos.y].pred.y = NULO;
	map[seedPos.x][seedPos.y].isSeed = true;
	map[seedPos.x][seedPos.y].skip = true;
	map[seedPos.x][seedPos.y].printPred = 0;

	return seedPos;
}

void Map::setBaseChance(Pos p)
{
	//*
	if(map[p.x][p.y].h == 6)
		map[p.x][p.y].chance = 50;

	else if(map[p.x][p.y].h == 5)
		map[p.x][p.y].chance = 60;

	else if(map[p.x][p.y].h == 4)
		map[p.x][p.y].chance = 70;

	else if(map[p.x][p.y].h == 3)
		map[p.x][p.y].chance = 80;	

	else if(map[p.x][p.y].h == 2)
		map[p.x][p.y].chance = 10;

	else if(map[p.x][p.y].h == 1)
		map[p.x][p.y].chance = 10;

	else if(map[p.x][p.y].h == 0)
		map[p.x][p.y].chance = 0;

	else
		map[p.x][p.y].chance = 50;
	//*/

	//map[p.x][p.y].chance = rand() % 101;
}

int Map::lowerChance(Pos oldP)
{
	int chance;
	//*
	if(map[oldP.x][oldP.y].h == 6)
		chance = map[oldP.x][oldP.y].chance * 0.40 * MULTIPLIER;

	else if(map[oldP.x][oldP.y].h == 5)
		chance = map[oldP.x][oldP.y].chance * 0.50 * MULTIPLIER;

	else if(map[oldP.x][oldP.y].h == 4)
		chance = map[oldP.x][oldP.y].chance * 0.60 * MULTIPLIER;

	else if(map[oldP.x][oldP.y].h == 3)
		chance = map[oldP.x][oldP.y].chance * 0.70 * MULTIPLIER;

	else if(map[oldP.x][oldP.y].h == 2)
		chance = map[oldP.x][oldP.y].chance * 0.1 * MULTIPLIER;

	else if(map[oldP.x][oldP.y].h == 1)
		chance = map[oldP.x][oldP.y].chance * 0.1 * MULTIPLIER;

	else if(map[oldP.x][oldP.y].h == 0)
		chance = map[oldP.x][oldP.y].chance * 0.0 * MULTIPLIER;

	else
		chance = map[oldP.x][oldP.y].chance * 0.5 * MULTIPLIER;
	//*/

	//chance =  map[oldP.x][oldP.y].chance * ((rand() % 101) / 100) * MULTIPLIER;

	return chance;
}