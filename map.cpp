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
/*void MapTile::setPos(int x, int y)
{
	pos.setPos(x, y);
}*/


MapTile::MapTile()
{
	h = MAX_H / 2 + 1;
	chance = 0;
	pred.setPos(NULO, NULO);
	isSeed = false;
	skip = false;
	visited = false;
}

/*Pos MapTile::getPos()
{
	return pos;
}

int MapTile::getPosX()
{
	return pos.getX();
}

int MapTile::getPosY()
{
	return pos.getY();
}*/

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

void MapTile::setBaseChance(){
	/*
	if(h == 6)
		chance = 50;

	else if(h == 5)
		chance = 60;

	else if(h == 4)
		chance = 70;

	else if(h == 3)
		chance = 80;	

	else if(h == 2)
		chance = 10;

	else if(h == 1)
		chance = 10;

	else if(h == 0)
		chance = 0;

	else
		chance = 50;
	//*/

	chance = rand() % 101;
}

void MapTile::lowerChance(MapTile prevTile)
{
	
	int oldChance = prevTile.getChance();
	/*int oldH = prevTile.getH();
	
	if(oldH == 6)
		chance = oldChance * 0.40 * MULTIPLIER;

	else if(oldH == 5)
		chance = oldChance * 0.50 * MULTIPLIER;

	else if(oldH == 4)
		chance = oldChance * 0.60 * MULTIPLIER;

	else if(oldH == 3)
		chance = oldChance * 0.70 * MULTIPLIER;

	else if(oldH == 2)
		chance = oldChance * 0.1 * MULTIPLIER;

	else if(oldH == 1)
		chance = oldChance * 0.1 * MULTIPLIER;

	else if(oldH == 0)
		chance = oldChance * 0.0 * MULTIPLIER;

	else
		chance = oldChance * 0.5 * MULTIPLIER;
	//*/

	chance =  oldChance * ((rand() % 101) / 100) * MULTIPLIER;
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

/*Map::Map()
{
	for(int y = 0; y < MAPSIZE; y++)
		for(int x = 0; x < MAPSIZE; x++)
		{
			Tile(x, y).setPos(x, y);
		}
}*/

bool Map::isPosInside(Pos p)
{
	if(p.getX() >= 0 && p.getY() >= 0 && p.getX() < MAPSIZE && p.getY() < MAPSIZE)
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


Pos Map::insertSeedHigh()
{
	Pos seedPos;
	int seedH;

	do{
		seedPos.setPos((rand() % MAPSIZE), (rand() % MAPSIZE ));
		seedH = rand() % MAX_H;
	} while(seedH < Tile(seedPos).getH()); // busca uma seed aleatória necessariamente com altura MAIOR do que tile

	Tile(seedPos).setH(seedH);
	Tile(seedPos).setBaseChance();
	Tile(seedPos).setPred(NULO, NULO);
	Tile(seedPos).setIsSeed(true);
	Tile(seedPos).setSkip(true);

	return seedPos;
}

Pos Map::insertSeedLow()
{
	Pos seedPos;
	int seedH;

	do{
		seedPos.setPos((rand() % MAPSIZE), (rand() % MAPSIZE ));
		seedH = rand() % MAX_H;
	} while(seedH > Tile(seedPos).getH()); // busca uma seed aleatória necessariamente com altura MENOR do que tile

	Tile(seedPos).setH(seedH);
	Tile(seedPos).setBaseChance();
	Tile(seedPos).setPred(NULO, NULO);
	Tile(seedPos).setIsSeed(true);
	Tile(seedPos).setSkip(true);

	return seedPos;
}

//*
void Map::insertHighArtifact()
{
	PosQueue currentQueue; //PosQueueLower	// Fila de posições

	PosBST nextPosTree;

	Pos auxPos = insertSeedHigh();

	int hCurrent = Tile(auxPos).getH(); // altura sendo trabalhada; começa com do seed

	currentQueue.insert(auxPos);

	// zera skips de altura abaixo da sendo trabalhada
	for(int i = 0; i < MAPSIZE; i++)
		for(int j = 0; j < MAPSIZE; j++)
			if(Tile(j, i).getH() < hCurrent)
				Tile(j, i).setSkip(false);

	for(int i = 0; i < MAPSIZE; i++)
		for(int j = 0; j < MAPSIZE; j++)
				Tile(j, i).setVisited(false);

	while(hCurrent > 0)
	{
		// checa toda a PosQueue
		while(!currentQueue.empty())
		{
			Pos queuePos = currentQueue.remove();

			// checa posições adjacentes a posição Current da PosQueue
			for(int yOffset = -1; yOffset <= 1; yOffset++)
				for(int xOffset = -1; xOffset <= 1; xOffset++)
				{
					Pos adjPos(queuePos.getX() + xOffset, queuePos.getY() + yOffset);

					if(isPosInside(adjPos) && hCurrent > Tile(adjPos).getH() && Tile(adjPos).getVisited() == 0) // adjacente está dentro do mapa e é menor que altura Current
					{
						Tile(adjPos).setPred(queuePos);
						Tile(adjPos).setVisited(true);

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

			//if(Tile(auxPos).getSkip() == false)
			//{
			//	Tile(auxPos).setSkip(true);
				Tile(auxPos).setH(hCurrent);
				Tile(auxPos).setBaseChance();
				
				currentQueue.insert(auxPos);
			//}
		}

		if(hCurrent == 0) // se chegou no fim das inserções dessa seed, zera PosQueue Current
			currentQueue.clearAll();
	} // enquanto currentQueue não estiver vazia
}


void Map::insertLowArtifact()
{
	PosQueue currentQueue;//, PosQueueLower;	// Fila de posições

	PosBST nextPosTree;

	Pos auxPos = insertSeedLow();

	int hCurrent = Tile(auxPos).getH(); // altura sendo trabalhada; começa com do seed

	currentQueue.insert(auxPos);

	// zera skips de altura abaixo da sendo trabalhada
	for(int i = 0; i < MAPSIZE; i++)
		for(int j = 0; j < MAPSIZE; j++)
			if(Tile(j, i).getH() > hCurrent)
				Tile(j, i).setSkip(false);

	for(int i = 0; i < MAPSIZE; i++)
		for(int j = 0; j < MAPSIZE; j++)
				Tile(j, i).setVisited(false);

	while(hCurrent < MAX_H)
	{
		// checa toda a PosQueue
		while(!currentQueue.empty())
		{
			Pos queuePos = currentQueue.remove();

			// checa posições adjacentes a posição Current da PosQueue
			for(int yOffset = -1; yOffset <= 1; yOffset++)
				for(int xOffset = -1; xOffset <= 1; xOffset++)
				{
					Pos adjPos(queuePos.getX() + xOffset, queuePos.getY() + yOffset);

					if(isPosInside(adjPos) && hCurrent < Tile(adjPos).getH() && Tile(adjPos).getVisited() == 0) // adjacente está dentro do mapa e é menor que altura Current
					{
						Tile(adjPos).setPred(queuePos);
						Tile(adjPos).setVisited(true);

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

			//if(Tile(auxPos).getSkip() == false)
			//{
			//	Tile(auxPos).setSkip(true);
				Tile(auxPos).setH(hCurrent);
				Tile(auxPos).setBaseChance();
				
				currentQueue.insert(auxPos);
			//}
		}

		if(hCurrent == 0) // se chegou no fim das inserções dessa seed, zera PosQueue Current
			currentQueue.clearAll();
	} // enquanto currentQueue não estiver vazia
}
//*/





















// IMPLEMENTAÇÃO DE INSERT ARTIFACTS COM DUAS FILAS EM VEZ DE BST

/*
void Map::insertHighArtifact()
{
	PosQueue currentQueue, PosQueueLower;	// Fila de posições

	Pos auxPos = insertSeedHigh();

	int hCurrent = Tile(auxPos).getH(); // altura sendo trabalhada; começa com do seed

	currentQueue.insert(auxPos);

	// zera skips de altura abaixo da sendo trabalhada
	for(int i = 0; i < MAPSIZE; i++)
		for(int j = 0; j < MAPSIZE; j++)
			if(Tile(j, i).getH() < hCurrent)
				Tile(j, i).setSkip(false);

	for(int i = 0; i < MAPSIZE; i++)
		for(int j = 0; j < MAPSIZE; j++)
				Tile(j, i).setVisited(false);

	while(hCurrent > 0)
	{
		// checa toda a PosQueue
		while(!currentQueue.empty())
		{
			Pos queuePos = currentQueue.remove();

			// checa posições adjacentes a posição Current da PosQueue
			for(int yOffset = -1; yOffset <= 1; yOffset++)
				for(int xOffset = -1; xOffset <= 1; xOffset++)
				{
					Pos adjPos(queuePos.getX() + xOffset, queuePos.getY() + yOffset);

					if(isPosInside(adjPos) && hCurrent > Tile(adjPos).getH() && Tile(adjPos).getVisited() == 0) // adjacente está dentro do mapa e é menor que altura Current
					{
						Tile(adjPos).setPred(queuePos);
						Tile(adjPos).setVisited(true);

						// diminui ou não altura da adjacente baseado na chance de manter do Current
						if(rand() % 100 <= Tile(queuePos).getChance()) // mantem altura e diminui chance dos próximos manterem
						{
							Tile(adjPos).setH(hCurrent);
							Tile(adjPos).lowerChance(Tile(queuePos));
							//Tile(adjPos).setSkip(true);

							currentQueue.insert(adjPos); // coloca tile de mesma altura na PosQueue de altura Current
						}

						else 
							PosQueueLower.insert(adjPos);
					}
				}
		}

		hCurrent--;

		// esvazia próxima PosQueue colocando membros não repetidos na PosQueue Current e setando altura/chance de manter
		while(!PosQueueLower.empty())
		{
			Pos auxPos = PosQueueLower.remove();

			if(Tile(auxPos).getSkip() == false)
			{
				Tile(auxPos).setSkip(true);
				Tile(auxPos).setH(hCurrent);
				Tile(auxPos).setBaseChance();
				
				currentQueue.insert(auxPos);
			}
		}

		if(hCurrent == 0) // se chegou no fim das inserções dessa seed, zera PosQueue Current
			currentQueue.clearAll();
	} // enquanto currentQueue não estiver vazia
}


void Map::insertLowArtifact()
{
	PosQueue currentQueue, PosQueueLower;	// Fila de posições

	//PosBST nextPosTree;

	Pos auxPos = insertSeedLow();

	int hCurrent = Tile(auxPos).getH(); // altura sendo trabalhada; começa com do seed

	currentQueue.insert(auxPos);

	// zera skips de altura abaixo da sendo trabalhada
	for(int i = 0; i < MAPSIZE; i++)
		for(int j = 0; j < MAPSIZE; j++)
			if(Tile(j, i).getH() > hCurrent)
				Tile(j, i).setSkip(false);

	for(int i = 0; i < MAPSIZE; i++)
		for(int j = 0; j < MAPSIZE; j++)
				Tile(j, i).setVisited(false);

	while(hCurrent < MAX_H)
	{
		// checa toda a PosQueue
		while(!currentQueue.empty())
		{
			Pos queuePos = currentQueue.remove();

			// checa posições adjacentes a posição Current da PosQueue
			for(int yOffset = -1; yOffset <= 1; yOffset++)
				for(int xOffset = -1; xOffset <= 1; xOffset++)
				{
					Pos adjPos(queuePos.getX() + xOffset, queuePos.getY() + yOffset);

					if(isPosInside(adjPos) && hCurrent < Tile(adjPos).getH() && Tile(adjPos).getVisited() == 0) // adjacente está dentro do mapa e é menor que altura Current
					{
						Tile(adjPos).setPred(queuePos);
						Tile(adjPos).setVisited(true);

						// diminui ou não altura da adjacente baseado na chance de manter do Current
						if(rand() % 100 <= Tile(queuePos).getChance()) // mantem altura e diminui chance dos próximos manterem
						{
							Tile(adjPos).setH(hCurrent);
							Tile(adjPos).lowerChance(Tile(queuePos));
							Tile(adjPos).setSkip(true);

							currentQueue.insert(adjPos); // coloca tile de mesma altura na PosQueue de altura Current
						}

						else 
							PosQueueLower.insert(adjPos);
					}
				}
		}

		hCurrent++;

		// esvazia próxima PosQueue colocando membros não repetidos na PosQueue Current e setando altura/chance de manter
		while(!PosQueueLower.empty())
		{
			Pos auxPos = PosQueueLower.remove();

			if(Tile(auxPos).getSkip() == false)
			{
				Tile(auxPos).setSkip(true);
				Tile(auxPos).setH(hCurrent);
				Tile(auxPos).setBaseChance();
				
				currentQueue.insert(auxPos);
			}
		}

		if(hCurrent == 0) // se chegou no fim das inserções dessa seed, zera PosQueue Current
			currentQueue.clearAll();
	} // enquanto currentQueue não estiver vazia
}
*/