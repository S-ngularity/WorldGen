#include <iostream>
#include <iomanip>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Map.h"

#include <SDL2/SDL.h>

using namespace std;

#define MULTIPLIER_SCREENSIZE 1

const int SCREEN_WIDTH = MAPWIDTH * MULTIPLIER_SCREENSIZE;
const int SCREEN_HEIGHT = MAPHEIGHT * MULTIPLIER_SCREENSIZE;

//The window we'll be rendering to
SDL_Window *Window = NULL;

//The window renderer
SDL_Renderer *Renderer = NULL;

SDL_Event event;

//funções SDL
bool SDLStart();
void SDLClose();

void erosion(int iteration, int numIts);
void tectonics(int iteration, int numIts);

void renderMap();
void renderMapNoSea();

Map map;
int seaLevel;
int highestH;

int main(int argc, char* args[])
{
	seaLevel = SEA;
	/*// seed manual (barra no começo dessa linha para mudar)
	int seed;
	cout << "Seed: ";
	cin >> seed;
	cout << endl;

	srand(seed);
	/*/srand(time(NULL));//*/

	if(!SDLStart())
	{
		printf("ERRO AO INICIALIZAR SDL");

		return -1;
	}

	renderMapNoSea();

	bool quit;
	bool readTectonics = true, readErosion = false;
	bool doTectonics = false, doErosion = false;
	int iteration, numIts, percentComplete;
	int printWhenMod = 20;

	//While application is running
	while(!quit)
	{
		if(readTectonics == true)
		{
			do{
				cout << endl <<  "Tectonics iterations (~30-60): ";
				cin >> numIts;
				cout << endl;
			}while(numIts < 0);

			readErosion = false;
			doErosion = false;
			iteration = 0;
			percentComplete = 0;

			if(numIts == 0)
			{
				readTectonics = true;
				doTectonics = false;
			}

			else
			{
				readTectonics = false;
				doTectonics = true;
			}
		}

		if(doTectonics == true)
		{
			tectonics(iteration, numIts);
			iteration++;

			if((int)(100 * ((float)iteration / (float)numIts)) > percentComplete)
			{

				percentComplete = (int)(100 * ((float)iteration / (float)numIts));

				cout << "\b\b\b" << percentComplete << "%";

				if(percentComplete % printWhenMod == 0)
					renderMapNoSea();
			}

			if(iteration == numIts)
			{
				doTectonics = false;
				readErosion = true;

				highestH = 0;

				for(int y = 0; y < MAPHEIGHT; y++)
					for(int x = 0; x < MAPWIDTH; x++)
						if(map.Tile(x, y).getH() > highestH)
							highestH = map.Tile(x, y).getH();

				cout << endl << endl << "Highest point: " << highestH << endl << endl;

				renderMapNoSea();
				SDL_RenderPresent(Renderer);
			}
		}

		if(readErosion == true)
		{
			do{
				cout << endl << "Erosion iterations (~500-5000): ";
				cin >> numIts;
				cout << endl;
			}while(numIts < 0);

			readTectonics = false;
			doTectonics = false;
			iteration = 0;
			percentComplete = 0;

			if(numIts == 0)
			{
				readErosion = false;
				doErosion = false;
				readTectonics = true;
			}

			else
			{
				readErosion = false;
				doErosion = true;
			}
		}

		if(doErosion == true)
		{
			erosion(iteration, numIts);
			iteration++;

			if((int)(100 * ((float)iteration / (float)numIts)) > percentComplete)
			{
				percentComplete = (int)(100 * ((float)iteration / (float)numIts));

				cout << "\b\b\b" << percentComplete << "%";

				if(percentComplete % printWhenMod == 0)
					renderMapNoSea();
			}

			if(iteration == numIts)
			{
				doErosion = false;
				readTectonics = false;

				highestH = 0;

				for(int y = 0; y < MAPHEIGHT; y++)
					for(int x = 0; x < MAPWIDTH; x++)
						if(map.Tile(x, y).getH() > highestH)
							highestH = map.Tile(x, y).getH();

				cout << endl << endl << "Highest point: " << highestH << endl << endl;
				cout << "Sea Level : " << setw(3) << setfill('0') << seaLevel;

				//renderMapNoSea();
				renderMap();
				SDL_RenderPresent(Renderer);
			}
		}
		
		//Handle events on queue
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					quit = true;
				break;

				case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
					{
						case SDLK_UP:
							if(!(event.key.keysym.mod & KMOD_SHIFT) && seaLevel + 1 < highestH)
							{
								seaLevel += 1;
								cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << "Sea Level : " << setw(3) << setfill('0') << seaLevel;
							}

							renderMap();
						break;

						case SDLK_DOWN:
							if(!(event.key.keysym.mod & KMOD_SHIFT) && seaLevel - 1 > 0)
							{
								seaLevel -= 1;
								cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << "Sea Level : " << setw(3) << setfill('0') << seaLevel;

								renderMap();
							}

							else
								renderMapNoSea();
						break;
					}
				break;
			}
		}

		SDL_RenderPresent(Renderer);
	}

	/*	int highestH = 0;
		
		for(int y = 0; y < MAPHEIGHT; y++)
			for(int x = 0; x < MAPWIDTH; x++)
				if(map.Tile(x, y).getH() > highestH)
					highestH = map.Tile(x, y).getH();

		cout << "highest tile = " << highestH << endl;
		cout << "sea = " << SEA << endl;
	*/

	/*// imprime erros de quando a diferença entre tiles adjacentes é maior que 1
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
							//map.Tile(nowPos).setError(true);
							printf("ERRO EM %d %d\n", x, y);
						}
					}
				}

		}//*/

	SDLClose();

	return 0;
}

void tectonics(int iteration, int numIts)
{
	Pos seedPos;

	float floatComplete = (float)iteration / (float)numIts;

	if(floatComplete < 0.1)
	{
		seedPos.setPos((rand() % MAPWIDTH), (rand() % MAPHEIGHT));
		map.insertHighArtifact(seedPos, 100);
	}

	else
	{
		if(rand() % 2 == 0)
		{
			seedPos.setPos((rand() % MAPWIDTH), (rand() % MAPHEIGHT));
			map.insertHighArtifact(seedPos, 100);
		}

		else
		{
			seedPos.setPos((rand() % MAPWIDTH), (rand() % MAPHEIGHT));

			map.insertLowArtifact(seedPos, 100);
		}
	}
}

void erosion(int iteration, int numIts)
{
	int multValue;
	Pos seedPos;

	float floatComplete = ((float)iteration / (float)numIts);

	if(floatComplete < 0.30)
		multValue = 30;

	else if(floatComplete < 0.60)
		multValue = 20;

	else
		multValue = 10;

	do{
		seedPos.setPos((rand() % MAPWIDTH), (rand() % MAPHEIGHT));
	}while(map.Tile(seedPos.getX(), seedPos.getY()).getH() <= SEA - SEA * 0.2);

	if(rand() % 2 == 0)
		map.insertHighArtifact(seedPos, multValue);

	else
		map.insertLowArtifact(seedPos, multValue);
}

void renderMap()
{
	//Clear screen
	SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
	SDL_RenderClear(Renderer);

	int baseColor = 100;
	float multiplierColor = (float)(255 - baseColor) / (highestH - seaLevel);

	for(int y = 0, contY = 0; y < MAPHEIGHT; contY++)
	{
		if(contY == MULTIPLIER_SCREENSIZE)
		{
			contY = 0;
			y++;
		}

		for(int x = 0, contX = 0; x < MAPWIDTH; contX++)
		{
			if(contX == MULTIPLIER_SCREENSIZE)
			{
				contX = 0;
				x++;
			}

			if(map.Tile(x, y).getError() == true)
				SDL_SetRenderDrawColor(Renderer, 100, 0, 0, 255);
			/*
			if(map.Tile(x, y).getIsSeed() == true && map.Tile(x, y).seedLow == true)
				SDL_SetRenderDrawColor(Renderer, 0, 255, 0, 255);

			else if(map.Tile(x, y).getIsSeed() == true && map.Tile(x, y).seedLow == false)
				SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);

			else
			{//*/

			else if(map.Tile(x, y).getH() <= seaLevel)
				SDL_SetRenderDrawColor(Renderer, 25, 45, 85, 255);//SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);//

			else
			{
				int hColor = (map.Tile(x, y).getH() - seaLevel) * multiplierColor;

				SDL_SetRenderDrawColor(Renderer, baseColor + hColor, baseColor + hColor, baseColor + hColor, 255);
			}

			SDL_RenderDrawPoint(Renderer, x * MULTIPLIER_SCREENSIZE + contX, y * MULTIPLIER_SCREENSIZE + contY);
		}
	}
}

void renderMapNoSea()
{
	//Clear screen
	SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
	SDL_RenderClear(Renderer);

	int baseColor = 0;
	float multiplierColor = (float)(255 - baseColor) / MAX_H;

	for(int y = 0, contY = 0; y < MAPHEIGHT; contY++)
	{
		if(contY == MULTIPLIER_SCREENSIZE)
		{
			contY = 0;
			y++;
		}

		for(int x = 0, contX = 0; x < MAPWIDTH; contX++)
		{
			if(contX == MULTIPLIER_SCREENSIZE)
			{
				contX = 0;
				x++;
			}

			if(map.Tile(x, y).getError() == true)
				SDL_SetRenderDrawColor(Renderer, 100, 0, 0, 255);
			/*
			if(map.Tile(x, y).getIsSeed() == true && map.Tile(x, y).seedLow == true)
				SDL_SetRenderDrawColor(Renderer, 0, 255, 0, 255);

			else if(map.Tile(x, y).getIsSeed() == true && map.Tile(x, y).seedLow == false)
				SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);

			else
			{//*/

			//else if(map.Tile(x, y).getH() <= SEA)
			//	SDL_SetRenderDrawColor(Renderer, 25, 45, 85, 255);//SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);//

			else
			{
				int hColor = map.Tile(x, y).getH() * multiplierColor;

				SDL_SetRenderDrawColor(Renderer, baseColor + hColor, baseColor + hColor, baseColor + hColor, 255);
			}

			SDL_RenderDrawPoint(Renderer, x * MULTIPLIER_SCREENSIZE + contX, y * MULTIPLIER_SCREENSIZE + contY);
		}
	}
}

bool SDLStart()
{
	//Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());

		return false;
	}

	//Create window
	Window = SDL_CreateWindow("WorldGen", 20, 40, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
	if(Window == NULL)
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());

		return false;
	}
	
	//Create renderer for window
	Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
	if(Renderer == NULL)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());

		return false;
	}

	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
	SDL_RenderSetLogicalSize(Renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

	//Initialize renderer color
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);

	return true;
}

void SDLClose()
{
	//Destroy window	
	SDL_DestroyRenderer(Renderer);
	SDL_DestroyWindow(Window);
	Window = NULL;
	Renderer = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}