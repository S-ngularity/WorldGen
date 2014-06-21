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
	visited = false;
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
	chance = prevTile.getChance() * ((rand() % 51 + 50) / 100) * MULTIPLIER;
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

bool MapTile::getVisited()
{
	return visited;
}

void MapTile::setVisited(bool newVisited)
{
	visited = newVisited;
}


// ----- ----- ----- ----- ----- ----- ----- ----- //

bool Map::isPosInside(Pos p)
{
	if(p.getX() >= 0 && p.getY() >= 0 && p.getX() < MAPWIDTH && p.getY() < MAPHEIGHT)
		return true;

	else
		return false;
}

MapTile& Map::Tile(Pos p)
{
	return map[p.getX()][p.getY()];
}

MapTile& Map::Tile(int x, int y)
{
	Pos tempPos(x, y);

	if(isPosInside(tempPos))
		return map[x][y];

	else
		return map[0][0];
}


Pos Map::insertSeedHigh(float highMultplier)
{
	Pos seedPos;
	int seedH;

	seedPos.setPos((rand() % MAPWIDTH), (rand() % MAPHEIGHT));

	int deltaRange = (MAX_H - Tile(seedPos).getH()) * (highMultplier / 100);
	
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

Pos Map::insertSeedLow(float lowMultiplier)
{
	Pos seedPos;
	int seedH;

	seedPos.setPos((rand() % MAPWIDTH), (rand() % MAPHEIGHT));

	int deltaRange = ((Tile(seedPos).getH() + 1)) * (lowMultiplier / 100);

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
void Map::insertHighArtifact(int deltaH)
{
	PosQueue currentQueue; //PosQueueLower	// Fila de posições

	PosBST nextPosTree;

	Pos auxPos = insertSeedHigh(deltaH);

	int hCurrent = Tile(auxPos).getH(); // altura sendo trabalhada; começa com do seed

	currentQueue.insert(auxPos);

	while(hCurrent > 0)
	{
		while(!currentQueue.empty()) // checa toda a PosQueue
		{
			Pos queuePos = currentQueue.remove();

			// checa posições adjacentes a posição Current da PosQueue
			for(int yOffset = -1; yOffset <= 1; yOffset++)
				for(int xOffset = -1; xOffset <= 1; xOffset++)
				{
					Pos adjPos(queuePos.getX() + xOffset, queuePos.getY() + yOffset);

					if(isPosInside(adjPos) && hCurrent > Tile(adjPos).getH() && Tile(adjPos).getSkip() == false) // adjacente está dentro do mapa e é menor que altura Current
					{
						Tile(adjPos).setPred(queuePos);
						Tile(adjPos).setSkip(true);

						// diminui ou não altura da adjacente baseado na chance de manter do Current
						if(rand() % 100 <= Tile(queuePos).getChance()) // mantem altura e diminui chance dos próximos manterem
						{
							Tile(adjPos).setH(hCurrent);
							Tile(adjPos).lowerChance(Tile(queuePos));
							//Tile(adjPos).setSkip(true);

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

		if(hCurrent == 0) // se chegou no fim das inserções dessa seed, zera PosQueue Current
			currentQueue.clearAll();
	} // enquanto currentQueue não estiver vazia

	for(int y = 0;  y < MAPHEIGHT; y++)
		for(int x = 0; x < MAPWIDTH; x++)
				Tile(x, y).setSkip(false);
}


void Map::insertLowArtifact(int deltaH)
{
	PosQueue currentQueue;//, PosQueueLower;	// Fila de posições

	PosBST nextPosTree;

	Pos auxPos = insertSeedLow(deltaH);

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

					if(isPosInside(adjPos) && hCurrent < Tile(adjPos).getH() && Tile(adjPos).getSkip() == false) // adjacente está dentro do mapa e é menor que altura Current
					{
						Tile(adjPos).setPred(queuePos);
						Tile(adjPos).setSkip(true);

						// diminui ou não altura da adjacente baseado na chance de manter do Current
						if(rand() % 100 <= Tile(queuePos).getChance()) // mantem altura e diminui chance dos próximos manterem
						{
							Tile(adjPos).setH(hCurrent);
							Tile(adjPos).lowerChance(Tile(queuePos));
							//Tile(adjPos).setSkip(true);

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