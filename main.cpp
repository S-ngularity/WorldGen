#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "map.h"

#include <SDL2/SDL.h>

using namespace std;

#define MULTIPLIER_SCREENSIZE 2

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

int main(int argc, char* args[])
{
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

	renderMap();

	bool quit;
	bool readTectonics = true, readErosion = false;
	bool doTectonics = false, doErosion = false;
	int iteration, numIts, percentComplete;
	int printWhenMod = 10;

	//While application is running
	while(!quit)
	{
		//Handle events on queue
		SDL_PollEvent(&event);

		if(readTectonics == true)
		{
			do{
				cout << endl <<  "Tectonics iterations (~30-60): " << endl;
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

				if(percentComplete % printWhenMod == 0)
					renderMapNoSea();

				cout << "\b\b\b" << percentComplete << "%";
			}

			if(iteration == numIts)
			{
				doTectonics = false;
				readErosion = true;
			}
		}

		if(readErosion == true)
		{
			do{
				cout << endl << "Erosion iterations (~500-5000): " << endl;
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

				if(percentComplete % printWhenMod == 0)
					renderMapNoSea();

				cout << "\b\b\b" << percentComplete << "%";
			}

			if(iteration == numIts)
			{
				doErosion = false;
				readTectonics = false;
			}
		}
		

		if(event.type == SDL_QUIT)
			quit = true;

		else if(event.type == SDL_KEYDOWN)
		{
			//Select surfaces based on key press
			switch(event.key.keysym.sym)
			{
				case SDLK_UP:
					renderMap();
				break;

				case SDLK_DOWN:
					renderMapNoSea();
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
			//do{
				seedPos.setPos((rand() % MAPWIDTH), (rand() % MAPHEIGHT));
			//}while(map.Tile(seedPos.getX(), seedPos.getY()).getH() <= SEA);

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
	int highestH = 0;
	
	for(int y = 0; y < MAPHEIGHT; y++)
		for(int x = 0; x < MAPWIDTH; x++)
			if(map.Tile(x, y).getH() > highestH)
				highestH = map.Tile(x, y).getH();

	//Clear screen
	SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
	SDL_RenderClear(Renderer);

	int baseColor = 100;
	int multiplierColor = (255 - baseColor) / (highestH - SEA);

	//Draw vertical line of yellow dots
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

			else if(map.Tile(x, y).getH() <= SEA)
				SDL_SetRenderDrawColor(Renderer, 25, 45, 85, 255);//SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);//

			else
			{
				int hColor = (map.Tile(x, y).getH() - SEA) * multiplierColor;

				SDL_SetRenderDrawColor(Renderer, baseColor + hColor, baseColor + hColor, baseColor + hColor, 255);
			}

			SDL_RenderDrawPoint(Renderer, x * MULTIPLIER_SCREENSIZE + contX, y * MULTIPLIER_SCREENSIZE + contY);
		}
	}
}

void renderMapNoSea()
{
	int highestH = 0;
	
	for(int y = 0; y < MAPHEIGHT; y++)
		for(int x = 0; x < MAPWIDTH; x++)
			if(map.Tile(x, y).getH() > highestH)
				highestH = map.Tile(x, y).getH();

	//Clear screen
	SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
	SDL_RenderClear(Renderer);

	int baseColor = 0;
	int multiplierColor = (255 - baseColor) / MAX_H;

	//Draw vertical line of yellow dots
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
	Window = SDL_CreateWindow("WorldGen", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
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