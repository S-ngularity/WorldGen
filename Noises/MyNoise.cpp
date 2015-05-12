#include "MyNoise.h"
#include "../Map.h"

#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <queue>

using namespace std;

MyNoise::MyNoise(Map *theMap)
{
	map = theMap;
	doneIts = 0;
	totalTecIts = 0;
	totalErsIts = 0;
	alreadySaved = false;
	state = running;

	srand(time(NULL));
}

void MyNoise::setMap(Map *m)
{
	map = m;
	reset();
}

void MyNoise::reset()
{
	doneIts = 0;
	totalTecIts = 0;
	totalErsIts = 0;
	state = running;
}

void MyNoise::runOnce()
{
	if(doneIts == 0)
	{
		readIterations();
		
		for(int y = 0; y < map->getMapHeight(); y++)
			for(int x = 0; x < map->getMapWidth(); x++)
				map->Tile(x, y).setH(INIT_H);

		map->setHighestH(INIT_H);
		map->setLowestH(INIT_H);

		map->setSeaLvl(SEA_LEVEL);
	}

	if(doneIts <= totalTecIts)
		tectonics();

	else if(doneIts <= totalTecIts + totalErsIts)
		erosion();

	doneIts++;

	checkIfFinished();
}

int MyNoise::getPercentComplete()
{
	return 100 * ((float)doneIts / (totalTecIts + totalErsIts));
}


void MyNoise::readIterations()
{
	do{
		cout << endl <<  "Tectonics iterations (~50-200): ";
		cin >> totalTecIts;
		cout << endl;
	}while(totalTecIts < 0);

	do{
		cout << endl << "Erosion iterations (~1500-50000): ";
		cin >> totalErsIts;
		cout << endl;
	}while(totalErsIts < 0);

	doneIts = 0;
	state = running;
}

void MyNoise::checkIfFinished()
{
	if(doneIts == totalTecIts + totalErsIts)
	{
		map->setHighestH(0);
		map->setLowestH(MAX_H);

		for(int y = 0; y < map->getMapHeight(); y++)
			for(int x = 0; x < map->getMapWidth(); x++)
			{

				if(map->Tile(x, y).getH() > map->getHighestH())
					map->setHighestH(map->Tile(x, y).getH());

				if(map->Tile(x, y).getH() < map->getLowestH())
					map->setLowestH(map->Tile(x, y).getH());
			}

		/* // imprime erros de quando a diferença entre tiles adjacentes é maior que 1
		for(int y = 0; y < map->getMapHeight(); y++)
			for(int x = 0; x < map->getMapWidth(); x++)
			{
				for(int yOffset = -1; yOffset <= 1; yOffset++)
					for(int xOffset = -1; xOffset <= 1; xOffset++)
					{
						Pos nowPos(x, y);
						Pos adjPos(x + xOffset, y + yOffset);

						if(map->isPosInsideWrap(adjPos))
						{
							if((map->Tile(adjPos).getH() - map->Tile(nowPos).getH() > 1) || (map->Tile(adjPos).getH() - map->Tile(nowPos).getH() < -1))
							{
								map->Tile(nowPos).setError(true);
								//printf("ERRO EM %d %d\n", x, y);
							}
						}
					}

			}//*/

		if(!alreadySaved) // SALVAR UMA VEZ RESULTADO EM TGA
		{
			unsigned char *imageData;
			imageData = (unsigned char*)malloc(sizeof(unsigned char) * map->getMapWidth() * map->getMapHeight());

			for(int y = 0; y < map->getMapHeight(); y++)
				for(int x = 0; x < map->getMapWidth(); x++)
				{
					if(map->Tile(x, y).getH() <= SEA_LEVEL)
						imageData[(map->getMapHeight() - 1 - y) * map->getMapWidth() + x] = 0;//(unsigned char)(((float)(SEA_LEVEL - 1) / MAX_H) * 256.0);

					else
						imageData[(map->getMapHeight() - 1 - y) * map->getMapWidth() + x] = (unsigned char)((int)((map->Tile(x, y).getH() - SEA_LEVEL) / (float)(MAX_H - SEA_LEVEL) * 255.0)); //(unsigned char)((int)(((float)map->Tile(x, y).getH() / MAX_H) * 256.0));
				}

			tgaSave("t.tga", map->getMapWidth(), map->getMapHeight(), 8, imageData);

			alreadySaved = true;
		}

		state = done;
	}
}

void MyNoise::tectonics()
{
	Pos seedPos;
	seedPos.setPos((rand() % map->getMapWidth()), (rand() % map->getMapHeight()));

	float floatComplete = (float)doneIts / totalTecIts;

	if(floatComplete < 0.1 || rand() % 2 == 0)
		insertHighArtifact(seedPos, 1);

	else
		insertLowArtifact(seedPos, 1);
}

void MyNoise::erosion()
{
	float rangeMultiplier;
	Pos seedPos;

	float floatComplete = ((float)doneIts / totalErsIts);

	if(floatComplete < 0.30)
		rangeMultiplier = 0.3;

	else if(floatComplete < 0.60)
		rangeMultiplier = 0.2;

	else
		rangeMultiplier = 0.1;


	if(rand() % 2 == 0)
	{
		// evita criar montanhas submarinas que não tenham possibilidade de virar ilhas (mas faz mapa tender a alturas maiores no geral)
		do{
			seedPos.setPos((rand() % map->getMapWidth()), (rand() % map->getMapHeight()));
		}while(map->Tile(seedPos.getX(), seedPos.getY()).getH() <= SEA_LEVEL - SEA_LEVEL * 0.5 * rangeMultiplier);

		insertHighArtifact(seedPos, rangeMultiplier);
	}

	else
	{
		// adicionei por simetria àcima, porém não sei porque torna (ou não) mapas melhores (mais altura entre mar e pico mais alto)
		//do{
			seedPos.setPos((rand() % map->getMapWidth()), (rand() % map->getMapHeight()));
		//}while(map->Tile(seedPos.getX(), seedPos.getY()).getH() <= SEA_LEVEL - SEA_LEVEL * 0.5);// - SEA_LEVEL * 0.5);

		insertLowArtifact(seedPos, rangeMultiplier);
	}
}


Pos MyNoise::insertSeedHigh(Pos seedPos, float rangeMultiplier)
{
	int seedH;

	int deltaRange;// = (MAX_H - map->Tile(seedPos).getH()) * (highMultplier / 100);

	if(map->Tile(seedPos).getH() > MAX_H / 2)
		deltaRange = (MAX_H - map->Tile(seedPos).getH()) * rangeMultiplier;

	else
		deltaRange = map->Tile(seedPos).getH() * rangeMultiplier;
	
	if(deltaRange < 1)
		deltaRange = 1;

	int deltaH = rand() % deltaRange;

	seedH = map->Tile(seedPos).getH() + deltaH;

	map->Tile(seedPos).setH(seedH);
	map->Tile(seedPos).setBaseChance();
	map->Tile(seedPos).setIsSeed(true);

	return seedPos;
}

Pos MyNoise::insertSeedLow(Pos seedPos, float rangeMultiplier)
{
	int seedH;

	int deltaRange;

	if(map->Tile(seedPos).getH() > MAX_H / 2)
		deltaRange = (MAX_H - map->Tile(seedPos).getH()) * rangeMultiplier;

	else
		deltaRange = map->Tile(seedPos).getH() * rangeMultiplier;

	if(deltaRange < 1)
		deltaRange = 1;

	int deltaH = rand() % deltaRange;

	seedH = map->Tile(seedPos).getH() - deltaH;

	map->Tile(seedPos).setH(seedH);
	map->Tile(seedPos).setBaseChance();
	map->Tile(seedPos).setIsSeed(true);

	return seedPos;
}

//*
void MyNoise::insertHighArtifact(Pos seedPos, float rangeMultiplier)
{
	queue<Pos> currentQueue, nextQueue;	// filas de posições

	Pos currentPos = insertSeedHigh(seedPos, rangeMultiplier);

	int hCurrent = map->Tile(currentPos).getH(); // altura sendo trabalhada; começa com do seed

	currentQueue.push(currentPos);

	while(hCurrent >= 0 && !currentQueue.empty())
	{
		while(!currentQueue.empty()) // checa toda a currentQueue
		{
			currentPos = currentQueue.front();
			currentQueue.pop();

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

				Pos adjPos(currentPos.getX() + xOffset, currentPos.getY() + yOffset);

				if(map->isPosInsideWrap(adjPos) &&		// adjacente está dentro do mapa
					map->Tile(adjPos).getH() < hCurrent)	// e é menor que altura hCurrent
				{
					// diminui ou não altura da adjacente baseado na chance de manter do hCurrent
					if(rand() % 100 <= map->Tile(currentPos).getChance()) // mantem altura e diminui chance dos próximos manterem
					{
						map->Tile(adjPos).setH(hCurrent); // também evita reroll
						map->Tile(adjPos).lowerChance(map->Tile(currentPos));

						currentQueue.push(adjPos); // coloca tile de mesma altura na currentQueue de altura Current
					}

					else
					{
						map->Tile(adjPos).setH(hCurrent); // "FLAG" PARA EVITAR REROLL, SERÁ REESCRITA COM ALTURA MENOR NA PASSAGEM DE NEXTQUEUE PRA CURRENTQUEUE
						nextQueue.push(adjPos); // insere na queue da próxima altura
					}
				}
			} // para todos os vizinhos
		} // de cada um dos tiles da current queue

		hCurrent--;

		// passa tiles da próxima altura pra fila atual
		while(!nextQueue.empty())
		{
			Pos auxPos = nextQueue.front();
			nextQueue.pop();

			map->Tile(auxPos).setH(hCurrent);
			map->Tile(auxPos).setBaseChance();
			
			currentQueue.push(auxPos);
		}
	} // enquanto não chegar no limite min/max de altura e tiver vizinhos válidos
}


void MyNoise::insertLowArtifact(Pos seedPos, float rangeMultiplier)
{
	queue<Pos> currentQueue, nextQueue;	// filas de posições

	Pos currentPos = insertSeedLow(seedPos, rangeMultiplier);

	int hCurrent = map->Tile(currentPos).getH(); // altura sendo trabalhada; começa com do seed

	currentQueue.push(currentPos);

	while(hCurrent < MAX_H && !currentQueue.empty())
	{
		while(!currentQueue.empty()) // checa toda a currentQueue
		{
			currentPos = currentQueue.front();
			currentQueue.pop();

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

				Pos adjPos(currentPos.getX() + xOffset, currentPos.getY() + yOffset);

				if(map->isPosInsideWrap(adjPos) && 				// adjacente está dentro do mapa
					map->Tile(adjPos).getH() > hCurrent)	// e é menor que altura hCurrent
				{
					// diminui ou não altura da adjacente baseado na chance de manter do hCurrent
					if(rand() % 100 <= map->Tile(currentPos).getChance()) // mantem altura e diminui chance dos próximos manterem
					{
						map->Tile(adjPos).setH(hCurrent);
						map->Tile(adjPos).lowerChance(map->Tile(currentPos));

						currentQueue.push(adjPos); // coloca tile de mesma altura na currentQueue de altura Current
					}

					else
					{
						map->Tile(adjPos).setH(hCurrent); // "FLAG" PARA EVITAR REROLL, SERÁ REESCRITA COM ALTURA MENOR NA PASSAGEM DE NEXTQUEUE PRA CURRENTQUEUE
						nextQueue.push(adjPos); // insere na queue da próxima altura
					}
				}
			} // para todos os vizinhos
		} // de cada um dos tiles da current queue

		hCurrent++;

		// passa tiles da próxima altura pra fila atual
		while(!nextQueue.empty())
		{
			Pos auxPos = nextQueue.front();
			nextQueue.pop();

			map->Tile(auxPos).setH(hCurrent);
			map->Tile(auxPos).setBaseChance();
			
			currentQueue.push(auxPos);
		}
	} // enquanto não chegar no limite min/max de altura e tiver vizinhos válidos
}
//*/

// função para salvar em tga, adaptada de "Antonio Ramires Fernandes ajbrf@yahoo.com" - http://www.lighthouse3d.com/opengl/terrain/index.php3?tgalib
int MyNoise::tgaSave(char const	*filename, 
			 short int		width, 
			 short int		height, 
			 unsigned char	pixelDepth,
			 unsigned char	*imageData) {

	unsigned char cGarbage = 0, type,mode,aux;
	short int iGarbage = 0;
	int i;
	FILE *file;

// open file and check for errors
	file = fopen(filename, "wb");
	if (file == NULL) {
		return(-1);
	}

// compute image type: 2 for RGB(A), 3 for greyscale
	mode = pixelDepth / 8;
	if ((pixelDepth == 24) || (pixelDepth == 32))
		type = 2;
	else
		type = 3;

// write the header
	fwrite(&cGarbage, sizeof(unsigned char), 1, file);
	fwrite(&cGarbage, sizeof(unsigned char), 1, file);

	fwrite(&type, sizeof(unsigned char), 1, file);

	fwrite(&iGarbage, sizeof(short int), 1, file);
	fwrite(&iGarbage, sizeof(short int), 1, file);
	fwrite(&cGarbage, sizeof(unsigned char), 1, file);
	fwrite(&iGarbage, sizeof(short int), 1, file);
	fwrite(&iGarbage, sizeof(short int), 1, file);

	fwrite(&width, sizeof(short int), 1, file);
	fwrite(&height, sizeof(short int), 1, file);
	fwrite(&pixelDepth, sizeof(unsigned char), 1, file);

	fwrite(&cGarbage, sizeof(unsigned char), 1, file);

// convert the image data from RGB(a) to BGR(A)
	if (mode >= 3)
	for (i=0; i < width * height * mode ; i+= mode) {
		aux = imageData[i];
		imageData[i] = imageData[i+2];
		imageData[i+2] = aux;
	}

// save the image data
	fwrite(imageData, sizeof(unsigned char), width * height * mode, file);
	fclose(file);
// release the memory
	free(imageData);

	return(0);
}
