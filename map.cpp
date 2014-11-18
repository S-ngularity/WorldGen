//#include <stdio.h>
#include <stdlib.h>
#include "Map.h"
#include "PosQueue.h"
#include "PosBst.h"

#define NULO -1

Map::Map()
{
	map = new MapTile*[MAPWIDTH];

	for(int i = 0; i < MAPWIDTH; i++)
		map[i] = new MapTile[MAPHEIGHT];

	for(int y = 0; y < MAPHEIGHT; y++)
		for(int x = 0; x < MAPWIDTH; x++)
			Tile(x, y).setH(INIT_H);
}

Map::~Map()
{
	for(int i = 0; i < MAPWIDTH; i++)
		delete [] map[i];

	delete [] map;
}

MapTile& Map::Tile(Pos p)
{
	return Tile(p.getX(), p.getY());
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

/*if(isPosInsideWrap(tempPos)) // WRAP IN Y (mudar isPosInsideWrap para só true)
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
		deltaRange = Tile(seedPos).getH() * (highMultiplier / 100);
	
	if(deltaRange < 1)
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
		deltaRange = Tile(seedPos).getH() * (lowMultiplier / 100);

	if(deltaRange < 1)
		deltaRange = 1;

	int deltaH = rand() % deltaRange;

	seedH = Tile(seedPos).getH() - deltaH;

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
	PosQueue currentQueue; //, PosQueueLower;	// Fila de posições

	PosBST nextPosTree;

	Pos auxPos = insertSeedHigh(seedPos, deltaH);

	int hCurrent = Tile(auxPos).getH(); // altura sendo trabalhada; começa com do seed

	currentQueue.insert(auxPos);

	while(hCurrent >= 0)
	{
		while(!currentQueue.isEmpty()) // checa toda a PosQueue
		{
			Pos queuePos = currentQueue.remove();

			for(int state = 1; state <= 4; state++)
			{
				int xOffset, yOffset;
				
				switch(state)
				{
					case 1:
						xOffset = 0;
						yOffset = -1;
					break;

					case 2:
						xOffset = 0;
						yOffset = 1;
					break;

					case 3:
						xOffset = -1;
						yOffset = 0;
					break;

					case 4:
						xOffset = 1;
						yOffset = 0;
					break;
				}

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
		while(!nextPosTree.isEmpty()) //!PosQueueLower.isEmpty())
		{
			Pos auxPos = nextPosTree.removeHead(); //nextPosTree.removeLowest(); //Pos auxPos = PosQueueLower.remove();

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
	PosQueue currentQueue; //, PosQueueLower;	// Fila de posições

	PosBST nextPosTree;

	Pos auxPos = insertSeedLow(seedPos, deltaH);

	int hCurrent = Tile(auxPos).getH(); // altura sendo trabalhada; começa com do seed

	currentQueue.insert(auxPos);

	while(hCurrent < MAX_H)
	{
		while(!currentQueue.isEmpty()) // checa toda a PosQueue
		{
			Pos queuePos = currentQueue.remove();

			for(int state = 1; state <= 4; state++)
			{
				int xOffset, yOffset;

				switch(state)
				{
					case 1:
						xOffset = 0;
						yOffset = -1;
					break;

					case 2:
						xOffset = 0;
						yOffset = 1;
					break;

					case 3:
						xOffset = -1;
						yOffset = 0;
					break;

					case 4:
						xOffset = 1;
						yOffset = 0;
					break;
				}
				
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
		while(!nextPosTree.isEmpty()) //!PosQueueLower.isEmpty())
		{
			Pos auxPos = nextPosTree.removeHead(); //nextPosTree.removeLowest(); //Pos auxPos = PosQueueLower.remove();

			Tile(auxPos).setH(hCurrent);
			Tile(auxPos).setBaseChance();
			Tile(auxPos).setSkip(false);

			currentQueue.insert(auxPos);
		}

		if(hCurrent == MAX_H) // se chegou no fim das inserções dessa seed, zera PosQueue Current
			currentQueue.clearAll();
	} // enquanto currentQueue não estiver vazia

	for(int y = 0;  y < MAPHEIGHT; y++)
		for(int x = 0; x < MAPWIDTH; x++)
				Tile(x, y).setSkip(false);
}
//*/