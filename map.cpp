#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include "filaPos.h"

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
/*void Tile::setPos(int x, int y)
{
	pos.setPos(x, y);
}*/


Tile::Tile()
{
	h = MAX_H / 2 + 1;
	chance = 0;
	pred.setPos(NULO, NULO);
	isSeed = false;
	skip = false;
}

/*Pos Tile::getPos()
{
	return pos;
}

int Tile::getPosX()
{
	return pos.getX();
}

int Tile::getPosY()
{
	return pos.getY();
}*/

// h
int Tile::getH()
{
	return h;
}

void Tile::setH(int newH)
{
	h = newH;
}


// chance
int Tile::getChance()
{
	return chance;
}

void Tile::setBaseChance(){
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

void Tile::lowerChance(Tile prevTile)
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
Pos Tile::getPred()
{
	return pred;
}

void Tile::setPred(Pos newPred)
{
	pred = newPred;
}

void Tile::setPred(int predX, int predY)
{
	pred.setPos(predX, predY);
}

// isSeed
bool Tile::getIsSeed()
{
	return isSeed;
}

void Tile::setIsSeed(bool newIsSeed)
{
	isSeed = newIsSeed;
}


// skip
bool Tile::getSkip()
{
	return skip;
}

void Tile::setSkip(bool newSkip)
{
	skip = newSkip;
}


// ----- ----- ----- ----- ----- ----- ----- ----- //

/*Map::Map()
{
	for(int y = 0; y < MAPSIZE; y++)
		for(int x = 0; x < MAPSIZE; x++)
		{
			getTile(x, y).setPos(x, y);
		}
}*/

bool Map::isPosInside(Pos p)
{
	if(p.getX() >= 0 && p.getY() >= 0 && p.getX() < MAPSIZE && p.getY() < MAPSIZE)
		return true;

	else
		return false;
}

Tile& Map::getTile(Pos p)
{
	return map[p.getX()][p.getY()];
}

Tile& Map::getTile(int x, int y)
{
	return map[x][y];
}


Pos Map::insertSeedHigh()
{
	Pos seedPos;
	int seedH;

	do{
		seedPos.setPos((rand() % MAPSIZE), (rand() % MAPSIZE ));
		seedH = rand() % MAX_H;
	} while(seedH < getTile(seedPos).getH()); // busca uma seed aleatória necessariamente com altura MAIOR do que tile

	getTile(seedPos).setH(seedH);
	getTile(seedPos).setBaseChance();
	getTile(seedPos).setPred(NULO, NULO);
	getTile(seedPos).setIsSeed(true);
	getTile(seedPos).setSkip(true);

	return seedPos;
}

Pos Map::insertSeedLow()
{
	Pos seedPos;
	int seedH;

	do{
		seedPos.setPos((rand() % MAPSIZE), (rand() % MAPSIZE ));
		seedH = rand() % MAX_H;
	} while(seedH > getTile(seedPos).getH()); // busca uma seed aleatória necessariamente com altura MENOR do que tile

	getTile(seedPos).setH(seedH);
	getTile(seedPos).setBaseChance();
	getTile(seedPos).setPred(NULO, NULO);
	getTile(seedPos).setIsSeed(true);
	getTile(seedPos).setSkip(true);

	return seedPos;
}

void Map::insertHighArtifact()
{
	Fila filaAtual, filaLower;	// filas de posições

	inicializa_fila(&filaAtual);
	inicializa_fila(&filaLower);

	Pos auxPos = insertSeedHigh();

	int hAtual = getTile(auxPos).getH(); // altura sendo trabalhada; começa com do seed

	insere_fila(&filaAtual, auxPos);

	// zera skips de altura abaixo da sendo trabalhada
	for(int i = 0; i < MAPSIZE; i++)
		for(int j = 0; j < MAPSIZE; j++)
			if(getTile(j, i).getH() < hAtual)
				getTile(j, i).setSkip(false);


	while(hAtual > 0)
	{
		// checa toda a fila
		while(!fila_vazia(&filaAtual))
		{
			Pos queuePos = remove_fila(&filaAtual);

			// checa posições adjacentes a posição atual da fila
			for(int yOffset = -1; yOffset <= 1; yOffset++)
				for(int xOffset = -1; xOffset <= 1; xOffset++)
				{
					Pos adjPos(queuePos.getX() + xOffset, queuePos.getY() + yOffset);

					if(isPosInside(adjPos) && hAtual > getTile(adjPos).getH()) /* && m[adjPos.x][adjPos.y].visitado == 0 */ // adjacente está dentro do mapa e é menor que altura atual
					{
						getTile(adjPos).setPred(queuePos);
						//getTile(adjPos).setVisitado(true);

						// diminui ou não altura da adjacente baseado na chance de manter do atual
						if(rand() % 100 <= getTile(queuePos).getChance()) // mantem altura e diminui chance dos próximos manterem
						{
							getTile(adjPos).setH(hAtual);
							getTile(adjPos).lowerChance(getTile(queuePos));
							getTile(adjPos).setSkip(true);

							insere_fila(&filaAtual, adjPos); // coloca tile de mesma altura na fila de altura atual
						}

						else 
							insere_fila(&filaLower, adjPos); // insere na fila da próxima altura (diminui altura)
					}
				}
		}

		hAtual--;

		// esvazia próxima fila colocando membros não repetidos na fila atual e setando altura/chance de manter
		while(!fila_vazia(&filaLower))
		{
			Pos auxPos = remove_fila(&filaLower);

			if(getTile(auxPos).getSkip() == false)
			{
				getTile(auxPos).setSkip(true);
				getTile(auxPos).setH(hAtual);
				getTile(auxPos).setBaseChance();
				
				insere_fila(&filaAtual, auxPos);
			}
		}

		// se chegou no fim das inserções dessa seed, zera fila atual
		if(hAtual == 0)
		{
			while(!fila_vazia(&filaAtual))
			{
				remove_fila(&filaAtual);
			}
		}
	} // enquanto filaAtual não estiver vazia
}


void Map::insertLowArtifact()
{
	Fila filaAtual, filaLower;	// filas de posições

	inicializa_fila(&filaAtual);
	inicializa_fila(&filaLower);

	Pos auxPos = insertSeedLow();

	int hAtual = getTile(auxPos).getH(); // altura sendo trabalhada; começa com do seed

	insere_fila(&filaAtual, auxPos);

	// zera skips de altura abaixo da sendo trabalhada
	for(int i = 0; i < MAPSIZE; i++)
		for(int j = 0; j < MAPSIZE; j++)
			if(getTile(j, i).getH() > hAtual)
				getTile(j, i).setSkip(false);


	while(hAtual < MAX_H)
	{
		// checa toda a fila
		while(!fila_vazia(&filaAtual))
		{
			Pos queuePos = remove_fila(&filaAtual);

			// checa posições adjacentes a posição atual da fila
			for(int yOffset = -1; yOffset <= 1; yOffset++)
				for(int xOffset = -1; xOffset <= 1; xOffset++)
				{
					Pos adjPos(queuePos.getX() + xOffset, queuePos.getY() + yOffset);

					if(isPosInside(adjPos) && hAtual < getTile(adjPos).getH()) /* && m[adjPos.x][adjPos.y].visitado == 0 */ // adjacente está dentro do mapa e é menor que altura atual
					{
						getTile(adjPos).setPred(queuePos);
						//getTile(adjPos).setVisitado(true);

						// diminui ou não altura da adjacente baseado na chance de manter do atual
						if(rand() % 100 <= getTile(queuePos).getChance()) // mantem altura e diminui chance dos próximos manterem
						{
							getTile(adjPos).setH(hAtual);
							getTile(adjPos).lowerChance(getTile(queuePos));
							getTile(adjPos).setSkip(true);

							insere_fila(&filaAtual, adjPos); // coloca tile de mesma altura na fila de altura atual
						}

						else 
							insere_fila(&filaLower, adjPos); // insere na fila da próxima altura (diminui altura)
					}
				}
		}

		hAtual++;

		// esvazia próxima fila colocando membros não repetidos na fila atual e setando altura/chance de manter
		while(!fila_vazia(&filaLower))
		{
			Pos auxPos = remove_fila(&filaLower);

			if(getTile(auxPos).getSkip() == false)
			{
				getTile(auxPos).setSkip(true);
				getTile(auxPos).setH(hAtual);
				getTile(auxPos).setBaseChance();
				
				insere_fila(&filaAtual, auxPos);
			}
		}

		// se chegou no fim das inserções dessa seed, zera fila atual
		if(hAtual == 0)
		{
			while(!fila_vazia(&filaAtual))
			{
				remove_fila(&filaAtual);
			}
		}
	} // enquanto filaAtual não estiver vazia
}