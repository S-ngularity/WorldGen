#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include "posQueue.h"
#include "posBST.h"

#define NULO -1

// ----- ----- ----- ----- ----- ----- ----- ----- //

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

int Pos::getX()
{
	return x;
}

int Pos::getY()
{
	return y;
}

void Pos::setPos(int newX, int newY)
{
	x = newX;
	y = newY;
}

// ----- ----- ----- ----- ----- ----- ----- ----- //

MapTile::MapTile()
{
	h = INIT_H;
	chance = 0;
	pred.setPos(NULO, NULO);
	isSeed = false;
	skip = false;
	error = false;
	seedLow = false;
}

// h
int MapTile::getH()
{
	return h;
}

void MapTile::setH(int newH)
{
	h = newH;
}


// chance
int MapTile::getChance()
{
	return chance;
}

void MapTile::setBaseChance()
{
	chance = rand() % 101;
}

void MapTile::lowerChance(MapTile prevTile)
{
	chance = prevTile.getChance() * ((rand() % 101) / 100) * MULTIPLIER;
}

// pred
Pos MapTile::getPred()
{
	return pred;
}

void MapTile::setPred(Pos newPred)
{
	pred = newPred;
}

void MapTile::setPred(int predX, int predY)
{
	pred.setPos(predX, predY);
}

// isSeed
bool MapTile::getIsSeed()
{
	return isSeed;
}

void MapTile::setIsSeed(bool newIsSeed)
{
	isSeed = newIsSeed;
}


// skip
bool MapTile::getSkip()
{
	return skip;
}

void MapTile::setSkip(bool newSkip)
{
	skip = newSkip;
}

bool MapTile::getError()
{
	return error;
}

void MapTile::setError(bool newError)
{
	error = newError;
}


// ----- ----- ----- ----- ----- ----- ----- ----- //

Map::Map()
{
	map = new MapTile*[MAPWIDTH];

	for(int i = 0; i < MAPWIDTH; i++)
		map[i] = new MapTile[MAPHEIGHT];
}

Map::~Map()
{
	for(int i = 0; i < MAPWIDTH; i++)
		delete [] map[i];

	delete [] map;
}

MapTile& Map::Tile(Pos p)
{
	if(isPosInsideWrap(p))
	{
		if(p.getX() < 0)
			p.setPos(MAPWIDTH + p.getX(), p.getY());

		else if(p.getX() >= MAPWIDTH)
			p.setPos(0 + (p.getX() - MAPWIDTH), p.getY());
	}

/*if(isPosInsideWrap(p)) // WRAP IN Y (mudar isPosInsideWrap para só true)
{
	if(p.getY() < 0)
		p.setPos(p.getX(), MAPHEIGHT + p.getY());

	else if(p.getY() >= MAPHEIGHT)
		p.setPos(p.getX(), 0 + (p.getY() - MAPHEIGHT));
}//*/

	if(isPosInsideNoWrap(p))
		return map[p.getX()][p.getY()];

	else
		return map[0][0];
}

MapTile& Map::Tile(int x, int y)
{
	Pos tempPos(x, y);

	if(isPosInsideWrap(tempPos))
	{
		if(tempPos.getX() < 0)
			tempPos.setPos(MAPWIDTH + tempPos.getX(), tempPos.getY());

		else if(tempPos.getX() >= MAPWIDTH)
			tempPos.setPos(0 + (tempPos.getX() - MAPWIDTH), tempPos.getY());
	}

/*if(isPosInsideWrap(tempPos)) // WRAP IN Y
{
	if(tempPos.getY() < 0)
		tempPos.setPos(tempPos.getX(), MAPHEIGHT + tempPos.getY());

	else if(tempPos.getY() >= MAPHEIGHT)
		tempPos.setPos(tempPos.getX(), 0 + (tempPos.getY() - MAPHEIGHT));
}//*/

	if(isPosInsideNoWrap(tempPos))
		return map[tempPos.getX()][tempPos.getY()];

	else
		return map[0][0];
}

bool Map::isPosInsideNoWrap(Pos p)
{
	if(p.getX() >= 0 && p.getY() >= 0 && p.getX() < MAPWIDTH && p.getY() < MAPHEIGHT)
		return true;

	else
		return false;
}	

bool Map::isPosInsideWrap(Pos p)
{
	if(p.getY() >= 0 && p.getY() < MAPHEIGHT)
		return true;

	else
		return false;
}	

Pos Map::insertSeedHigh(Pos seedPos, float highMultiplier)
{
	int seedH;

	int deltaRange;// = (MAX_H - Tile(seedPos).getH()) * (highMultplier / 100);

	if(Tile(seedPos).getH() > MAX_H / 2)
		deltaRange = (MAX_H - Tile(seedPos).getH()) * (highMultiplier / 100);

	else
		deltaRange = (Tile(seedPos).getH() + 1) * (highMultiplier / 100);
	
	if(deltaRange == 0)
		deltaRange = 1;

	int deltaH = rand() % deltaRange;

	seedH = Tile(seedPos).getH() + deltaH;

	Tile(seedPos).setH(seedH);
	Tile(seedPos).setBaseChance();
	Tile(seedPos).setPred(NULO, NULO);
	Tile(seedPos).setIsSeed(true);
	Tile(seedPos).setSkip(true);

	return seedPos;
}

Pos Map::insertSeedLow(Pos seedPos, float lowMultiplier)
{
	int seedH;

	int deltaRange;

	if(Tile(seedPos).getH() > MAX_H / 2)
		deltaRange = (MAX_H - Tile(seedPos).getH()) * (lowMultiplier / 100);

	else
		deltaRange = (Tile(seedPos).getH() + 1) * (lowMultiplier / 100);

	if(deltaRange < 1)
		deltaRange = 1;

	int deltaH = rand() % deltaRange;

	seedH = Tile(seedPos).getH() - deltaH;

Tile(seedPos).seedLow = true;
	Tile(seedPos).setH(seedH);
	Tile(seedPos).setBaseChance();
	Tile(seedPos).setPred(NULO, NULO);
	Tile(seedPos).setIsSeed(true);
	Tile(seedPos).setSkip(true);

	return seedPos;
}

//*
void Map::insertHighArtifact(Pos seedPos, int deltaH)
{
	PosQueue currentQueue; //PosQueueLower	// Fila de posições

	PosBST nextPosTree;

	Pos auxPos = insertSeedHigh(seedPos, deltaH);

	int hCurrent = Tile(auxPos).getH(); // altura sendo trabalhada; começa com do seed

	currentQueue.insert(auxPos);

	while(hCurrent >= 0)
	{
		while(!currentQueue.empty()) // checa toda a PosQueue
		{
			Pos queuePos = currentQueue.remove();

			// checa posições adjacentes a posição Current da PosQueue
			for(int yOffset = -1; yOffset <= 1; yOffset++)
				for(int xOffset = -1; xOffset <= 1; xOffset++)
				{
					Pos adjPos(queuePos.getX() + xOffset, queuePos.getY() + yOffset);

					if(isPosInsideWrap(adjPos) && hCurrent > Tile(adjPos).getH() && Tile(adjPos).getSkip() == false) // adjacente está dentro do mapa e é menor que altura Current
					{
						Tile(adjPos).setPred(queuePos);
						//Tile(adjPos).setSkip(true);

						// diminui ou não altura da adjacente baseado na chance de manter do Current
						if(rand() % 100 <= Tile(queuePos).getChance()) // mantem altura e diminui chance dos próximos manterem
						{
							Tile(adjPos).setH(hCurrent);
							Tile(adjPos).lowerChance(Tile(queuePos));
							Tile(adjPos).setSkip(true);

							currentQueue.insert(adjPos); // coloca tile de mesma altura na PosQueue de altura Current
						}

						else 
							nextPosTree.insert(adjPos); // insere na PosQueue da próxima altura (diminui altura)
							//PosQueueLower.insert(adjPos);
					}
				}
		}

		hCurrent--;

		// esvazia próxima PosQueue colocando membros não repetidos na PosQueue Current e setando altura/chance de manter
		while(!nextPosTree.empty()) //!PosQueueLower.empty())
		{
			Pos auxPos = nextPosTree.removeLowest(); //Pos auxPos = PosQueueLower.remove();

			Tile(auxPos).setH(hCurrent);
			Tile(auxPos).setBaseChance();
			Tile(auxPos).setSkip(false);
			
			currentQueue.insert(auxPos);
		}

		if(hCurrent < 0) // se chegou no fim das inserções dessa seed, zera PosQueue Current
			currentQueue.clearAll();
	} // enquanto currentQueue não estiver vazia

	for(int y = 0;  y < MAPHEIGHT; y++)
		for(int x = 0; x < MAPWIDTH; x++)
				Tile(x, y).setSkip(false);
}


void Map::insertLowArtifact(Pos seedPos, int deltaH)
{
	PosQueue currentQueue;//, PosQueueLower;	// Fila de posições

	PosBST nextPosTree;

	Pos auxPos = insertSeedLow(seedPos, deltaH);

	int hCurrent = Tile(auxPos).getH(); // altura sendo trabalhada; começa com do seed

	currentQueue.insert(auxPos);

	while(hCurrent < MAX_H)
	{
		while(!currentQueue.empty()) // checa toda a PosQueue
		{
			Pos queuePos = currentQueue.remove();

			// checa posições adjacentes a posição Current da PosQueue
			for(int yOffset = -1; yOffset <= 1; yOffset++)
				for(int xOffset = -1; xOffset <= 1; xOffset++)
				{
					Pos adjPos(queuePos.getX() + xOffset, queuePos.getY() + yOffset);

					if(isPosInsideWrap(adjPos) && hCurrent < Tile(adjPos).getH() && Tile(adjPos).getSkip() == false) // adjacente está dentro do mapa e é menor que altura Current
					{
						Tile(adjPos).setPred(queuePos);
						//Tile(adjPos).setSkip(true);

						// diminui ou não altura da adjacente baseado na chance de manter do Current
						if(rand() % 100 <= Tile(queuePos).getChance()) // mantem altura e diminui chance dos próximos manterem
						{
							Tile(adjPos).setH(hCurrent);
							Tile(adjPos).lowerChance(Tile(queuePos));
							Tile(adjPos).setSkip(true);

							currentQueue.insert(adjPos); // coloca tile de mesma altura na PosQueue de altura Current
						}

						else 
							nextPosTree.insert(adjPos); // insere na PosQueue da próxima altura (diminui altura)
							//PosQueueLower.insert(adjPos);
					}
				}
		}

		hCurrent++;

		// esvazia próxima PosQueue colocando membros não repetidos na PosQueue Current e setando altura/chance de manter
		while(!nextPosTree.empty()) //!PosQueueLower.empty())
		{
			Pos auxPos = nextPosTree.removeLowest(); //Pos auxPos = PosQueueLower.remove();

			Tile(auxPos).setH(hCurrent);
			Tile(auxPos).setBaseChance();
			Tile(auxPos).setSkip(false);

			currentQueue.insert(auxPos);
		}

		if(hCurrent == 0) // se chegou no fim das inserções dessa seed, zera PosQueue Current
			currentQueue.clearAll();
	} // enquanto currentQueue não estiver vazia

	for(int y = 0;  y < MAPHEIGHT; y++)
		for(int x = 0; x < MAPWIDTH; x++)
				Tile(x, y).setSkip(false);
}
//*/