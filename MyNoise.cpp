#include <iostream>
#include <iomanip>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "MyNoise.h"

#include "Map.h"
#include "MapTile.h"
#include "Pos.h"
#include "PosQueue.h"

using namespace std;

MyNoise::MyNoise(Map &theMap) : map(theMap)
{
	alreadySaved = false;
	state = readTect;
	shouldUpdateScreen = NO;

	srand(time(NULL));
}



void MyNoise::readTectonics()
{
	do{
		cout << endl <<  "Tectonics iterations (~50-200): ";
		cin >> numIts;
		cout << endl;
	}while(numIts < 0);

	if(numIts == 0)
		state = readEro;

	else
		state = doTect;
	
	iteration = 0;
	percentComplete = 0;
}

void MyNoise::doTectonics()
{
	tectonics();
	iteration++;

	updatePercent();
	checkIfFinished();
}

void MyNoise::readErosion()
{
	do{
		cout << endl << "Erosion iterations (~1500-50000): ";
		cin >> numIts;
		cout << endl;
	}while(numIts < 0);

	if(numIts == 0)
		state = done;

	else
		state = doEro;

	iteration = 0;
	percentComplete = 0;
}

void MyNoise::doErosion()
{
	erosion();
	iteration++;

	updatePercent();
	checkIfFinished();
}



void MyNoise::updatePercent()
{
	if((int)(100 * ((float)iteration / numIts)) > percentComplete)
	{
		percentComplete = 100 * ((float)iteration / numIts);

		cout << "\b\b\b" << percentComplete << "%";

		if(percentComplete % UPDATE_AT_PERCENT == 0)
			shouldUpdateScreen = WITHOUT_SEA; //renderMapNoSea();
	}
}

void MyNoise::checkIfFinished()
{
	if(iteration == numIts && (state == doTect || state == doEro))
	{
		highestH = 0;

		for(int y = 0; y < MAPHEIGHT; y++)
			for(int x = 0; x < MAPWIDTH; x++)
				if(map.Tile(x, y).getH() > highestH)
					highestH = map.Tile(x, y).getH();

		cout << endl << endl << "Highest point: " << highestH << endl << endl;

		if(state == doTect)
		{
			shouldUpdateScreen = WITHOUT_SEA; //renderMapNoSea();
											  //SDL_RenderPresent(Renderer);

			state = readEro;
		}

		else if (state == doEro)
		{
			cout << "Sea Level : " << setw(3) << setfill('0') << SEA;

			/* // imprime erros de quando a diferença entre tiles adjacentes é maior que 1
			for(int y = 0; y < MAPHEIGHT; y++)
				for(int x = 0; x < MAPWIDTH; x++)
				{
					for(int yOffset = -1; yOffset <= 1; yOffset++)
						for(int xOffset = -1; xOffset <= 1; xOffset++)
						{
							Pos nowPos(x, y);
							Pos adjPos(x + xOffset, y + yOffset);

							if(map.isPosInsideWrap(adjPos))
							{
								if((map.Tile(adjPos).getH() - map.Tile(nowPos).getH() > 1) || (map.Tile(adjPos).getH() - map.Tile(nowPos).getH() < -1))
								{
									map.Tile(nowPos).setError(true);
									//printf("ERRO EM %d %d\n", x, y);
								}
							}
						}

				}//*/

			shouldUpdateScreen = WITH_SEA; //renderMap();
										   //SDL_RenderPresent(Renderer);

			state = done;
		}
	}
}



void MyNoise::runOnce()
{
	switch(state)
	{
		case readTect:
			readTectonics();
		break;

		case doTect:
			doTectonics();
		break;

		case readEro:
			readErosion();
		break;

		case doEro:
			doErosion();
		break;

		case done:
			if(!alreadySaved) // SALVAR UMA VEZ RESULTADO EM TGA
			{
				unsigned char *imageData;
				imageData = (unsigned char*)malloc(sizeof(unsigned char) * MAPWIDTH * MAPHEIGHT);

				for(int y = 0; y < MAPHEIGHT; y++)
					for(int x = 0; x < MAPWIDTH; x++)
						imageData[y * MAPWIDTH + x] = (unsigned char)((int)(((float)map.Tile(x, y).getH() / MAX_H) * 256.0));

				tgaSave("t.tga", MAPWIDTH, MAPHEIGHT, 8, imageData);

				alreadySaved = true;

			}
		break;
	}
}

int MyNoise::askingForScreenUpdate()
{
	int temp = shouldUpdateScreen;

	shouldUpdateScreen = NO;

	return temp;
}

int MyNoise::getHighestH()
{
	return highestH;
}



void MyNoise::tectonics()
{
	Pos seedPos;

	float floatComplete = (float)iteration / numIts;

	if(floatComplete < 0.1)
	{
		seedPos.setPos((rand() % MAPWIDTH), (rand() % MAPHEIGHT));
		insertHighArtifact(seedPos, 1);
	}

	else
	{
		if(rand() % 2 == 0)
		{
			seedPos.setPos((rand() % MAPWIDTH), (rand() % MAPHEIGHT));
			insertHighArtifact(seedPos, 1);
		}

		else
		{
			seedPos.setPos((rand() % MAPWIDTH), (rand() % MAPHEIGHT));

			insertLowArtifact(seedPos, 1);
		}
	}
}

void MyNoise::erosion()
{
	float rangeMultiplier;
	Pos seedPos;

	float floatComplete = ((float)iteration / numIts);

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
			seedPos.setPos((rand() % MAPWIDTH), (rand() % MAPHEIGHT));
		}while(map.Tile(seedPos.getX(), seedPos.getY()).getH() <= SEA - SEA * 0.5 * rangeMultiplier);

		insertHighArtifact(seedPos, rangeMultiplier);
	}

	else
	{
		// adicionei por simetria àcima, porém não sei porque torna (ou não) mapas melhores (mais altura entre mar e pico mais alto)
		//do{
			seedPos.setPos((rand() % MAPWIDTH), (rand() % MAPHEIGHT));
		//}while(map.Tile(seedPos.getX(), seedPos.getY()).getH() <= SEA - SEA * 0.5);// - SEA * 0.5);

		insertLowArtifact(seedPos, rangeMultiplier);
	}
}






Pos MyNoise::insertSeedHigh(Pos seedPos, float rangeMultiplier)
{
	int seedH;

	int deltaRange;// = (MAX_H - map.Tile(seedPos).getH()) * (highMultplier / 100);

	if(map.Tile(seedPos).getH() > MAX_H / 2)
		deltaRange = (MAX_H - map.Tile(seedPos).getH()) * rangeMultiplier;

	else
		deltaRange = map.Tile(seedPos).getH() * rangeMultiplier;
	
	if(deltaRange < 1)
		deltaRange = 1;

	int deltaH = rand() % deltaRange;

	seedH = map.Tile(seedPos).getH() + deltaH;

	map.Tile(seedPos).setH(seedH);
	map.Tile(seedPos).setBaseChance();
	map.Tile(seedPos).setIsSeed(true);

	return seedPos;
}

Pos MyNoise::insertSeedLow(Pos seedPos, float rangeMultiplier)
{
	int seedH;

	int deltaRange;

	if(map.Tile(seedPos).getH() > MAX_H / 2)
		deltaRange = (MAX_H - map.Tile(seedPos).getH()) * rangeMultiplier;

	else
		deltaRange = map.Tile(seedPos).getH() * rangeMultiplier;

	if(deltaRange < 1)
		deltaRange = 1;

	int deltaH = rand() % deltaRange;

	seedH = map.Tile(seedPos).getH() - deltaH;

	map.Tile(seedPos).setH(seedH);
	map.Tile(seedPos).setBaseChance();
	map.Tile(seedPos).setIsSeed(true);

	return seedPos;
}

//*
void MyNoise::insertHighArtifact(Pos seedPos, float rangeMultiplier)
{
	PosQueue currentQueue, nextQueue;	// filas de posições

	Pos currentPos = insertSeedHigh(seedPos, rangeMultiplier);

	int hCurrent = map.Tile(currentPos).getH(); // altura sendo trabalhada; começa com do seed

	currentQueue.insert(currentPos);

	while(hCurrent >= 0 && !currentQueue.isEmpty())
	{
		while(!currentQueue.isEmpty()) // checa toda a currentQueue
		{
			currentPos = currentQueue.remove();

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

				if(map.isPosInsideWrap(adjPos)				// adjacente está dentro do mapa
					&& hCurrent > map.Tile(adjPos).getH())	// e é menor que altura hCurrent
				{
					map.Tile(adjPos).setPred(currentPos);

					// diminui ou não altura da adjacente baseado na chance de manter do hCurrent
					if(rand() % 100 <= map.Tile(currentPos).getChance()) // mantem altura e diminui chance dos próximos manterem
					{
						map.Tile(adjPos).setH(hCurrent); // também evita reroll
						map.Tile(adjPos).lowerChance(map.Tile(currentPos));

						currentQueue.insert(adjPos); // coloca tile de mesma altura na currentQueue de altura Current
					}

					else
					{
						map.Tile(adjPos).setH(hCurrent); // "FLAG" PARA EVITAR REROLL, SERÁ REESCRITA COM ALTURA MENOR NA PASSAGEM DE NEXTQUEUE PRA CURRENTQUEUE
						nextQueue.insert(adjPos); // insere na queue da próxima altura
					}
				}
			} // para todos os vizinhos
		} // de cada um dos tiles da current queue

		hCurrent--;

		// passa tiles da próxima altura pra fila atual
		while(!nextQueue.isEmpty())
		{
			Pos auxPos = nextQueue.remove();

			map.Tile(auxPos).setH(hCurrent);
			map.Tile(auxPos).setBaseChance();
			
			currentQueue.insert(auxPos);
		}
	} // enquanto não chegar no limite min/max de altura e tiver vizinhos válidos
}


void MyNoise::insertLowArtifact(Pos seedPos, float rangeMultiplier)
{
	PosQueue currentQueue, nextQueue;	// filas de posições

	Pos currentPos = insertSeedLow(seedPos, rangeMultiplier);

	int hCurrent = map.Tile(currentPos).getH(); // altura sendo trabalhada; começa com do seed

	currentQueue.insert(currentPos);

	while(hCurrent < MAX_H && !currentQueue.isEmpty())
	{
		while(!currentQueue.isEmpty()) // checa toda a currentQueue
		{
			currentPos = currentQueue.remove();

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

				if(map.isPosInsideWrap(adjPos)				// adjacente está dentro do mapa
					&& hCurrent < map.Tile(adjPos).getH())	// e é menor que altura hCurrent
				{
					map.Tile(adjPos).setPred(currentPos);

					// diminui ou não altura da adjacente baseado na chance de manter do hCurrent
					if(rand() % 100 <= map.Tile(currentPos).getChance()) // mantem altura e diminui chance dos próximos manterem
					{
						map.Tile(adjPos).setH(hCurrent);
						map.Tile(adjPos).lowerChance(map.Tile(currentPos));

						currentQueue.insert(adjPos); // coloca tile de mesma altura na currentQueue de altura Current
					}

					else
					{
						map.Tile(adjPos).setH(hCurrent); // "FLAG" PARA EVITAR REROLL, SERÁ REESCRITA COM ALTURA MENOR NA PASSAGEM DE NEXTQUEUE PRA CURRENTQUEUE
						nextQueue.insert(adjPos); // insere na queue da próxima altura
					}
				}
			} // para todos os vizinhos
		} // de cada um dos tiles da current queue

		hCurrent++;

		// passa tiles da próxima altura pra fila atual
		while(!nextQueue.isEmpty())
		{
			Pos auxPos = nextQueue.remove();

			map.Tile(auxPos).setH(hCurrent);
			map.Tile(auxPos).setBaseChance();
			
			currentQueue.insert(auxPos);
		}
	} // enquanto não chegar no limite min/max de altura e tiver vizinhos válidos
}
//*/
/*
void MyNoise::insertHighArtifact(Pos seedPos, int deltaH)
{
	PosQueue currentQueue; //, PosQueueLower;	// Fila de posições

	PosBST nextTree;

	Pos auxPos = insertSeedHigh(seedPos, deltaH);

	currentQueue.insert(auxPos);

	while(!currentQueue.isEmpty()) // checa toda a PosQueue
	{
		Pos currentPos = currentQueue.remove();

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

			if(map.isPosInsideWrap(adjPos)							 // adjacente está dentro do mapa
				//&& map.Tile(adjPos).getSkip() == false				 // e ainda não foi rolado
				&& map.Tile(currentPos).getH() > map.Tile(adjPos).getH() // e é menor que atual
				&& map.Tile(currentPos).getH() >= 1)				 // e não vai ter chance de ficar menos que 0
			{
				map.Tile(adjPos).setPred(currentPos);
				map.Tile(adjPos).setSkip(true); // proibe próximas iterações de tentarem dimiuir a altura desse tile

				// diminui ou não altura da adjacente baseado na chance de manter do Current
				if(rand() % 100 <= map.Tile(currentPos).getChance()) // mantem altura e diminui chance dos próximos manterem
				{
					map.Tile(adjPos).setH(map.Tile(currentPos).getH());
					map.Tile(adjPos).lowerChance(map.Tile(currentPos));
				}

				else 
				{
					map.Tile(adjPos).setH(map.Tile(currentPos).getH() - 1);
					map.Tile(adjPos).setBaseChance();
				}

				currentQueue.insert(adjPos); // coloca tile de mesma altura na PosQueue de altura Current
			}
		}
	}

	currentQueue.clearAll();

	for(int y = 0;  y < MAPHEIGHT; y++)
		for(int x = 0; x < MAPWIDTH; x++)
				map.Tile(x, y).setSkip(false);
}

void MyNoise::insertLowArtifact(Pos seedPos, int deltaH)
{
	PosQueue currentQueue; //, PosQueueLower;	// Fila de posições

	PosBST nextTree;

	Pos auxPos = insertSeedLow(seedPos, deltaH);

	currentQueue.insert(auxPos);

	while(!currentQueue.isEmpty()) // checa toda a PosQueue
	{
		Pos currentPos = currentQueue.remove();

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

			if(map.isPosInsideWrap(adjPos)							 // adjacente está dentro do mapa
				//&& map.Tile(adjPos).getSkip() == false				 // e ainda não foi rolado
				&& map.Tile(currentPos).getH() < map.Tile(adjPos).getH() // e é maior que atual
				&& map.Tile(currentPos).getH() <= MAX_H - 1)		 // e não vai ter chance de ficar menos que 0
			{
				map.Tile(adjPos).setPred(currentPos);
				map.Tile(adjPos).setSkip(true); // proibe próximas iterações de tentarem dimiuir a altura desse tile

				// diminui ou não altura da adjacente baseado na chance de manter do Current
				if(rand() % 100 <= map.Tile(currentPos).getChance()) // mantem altura e diminui chance dos próximos manterem
				{
					map.Tile(adjPos).setH(map.Tile(currentPos).getH());
					map.Tile(adjPos).lowerChance(map.Tile(currentPos));
				}

				else 
				{
					map.Tile(adjPos).setH(map.Tile(currentPos).getH() + 1);
					map.Tile(adjPos).setBaseChance();
				}

				currentQueue.insert(adjPos); // coloca tile de mesma altura na PosQueue de altura Current
			}
		}
	}

	currentQueue.clearAll();

	for(int y = 0;  y < MAPHEIGHT; y++)
		for(int x = 0; x < MAPWIDTH; x++)
				map.Tile(x, y).setSkip(false);
}
//*/


// função para salvar em tga, adaptada de "Antonio Ramires Fernandes ajbrf@yahoo.com" - http://www.lighthouse3d.com/opengl/terrain/index.php3?tgalib
int MyNoise::tgaSave(char			*filename, 
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
