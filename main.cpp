#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "map.h"

#include <SDL2/SDL.h>

using namespace std;

#define MULTIPLIER_SCREENSIZE 3
#define MULTIPLIER_COLOR 255 / MAX_H

const int SCREEN_WIDTH = MAPSIZE * MULTIPLIER_SCREENSIZE;
const int SCREEN_HEIGHT = MAPSIZE * MULTIPLIER_SCREENSIZE;

//The window we'll be rendering to
SDL_Window *Window = NULL;

//The window renderer
SDL_Renderer *Renderer = NULL;

//funções SDL
bool SDLStart();
void SDLClose();

Map map;

int main(int argc, char* args[])
{
	int numIts; // iterações de inserção de artefatos

	/*// seed manual (barra no começo dessa linha para mudar)
	int seed;
	cout << "Seed: ";
	cin >> seed;
	cout << endl;

	srand(seed);
	/*/srand(time(NULL));//*/

	cout << "Iterations: ";
	cin >> numIts;
	while(numIts < 1)
	{
		cout << "Number of iterations must be higher than 0: " << endl;
		cin >> numIts;
	}
	cout << endl;

	float multHigh = 100, multLow = 100;

	for(int iterations = 0; iterations < numIts; iterations++)
	{
		if((float)iterations / numIts > 0.8)
			{multHigh = multLow = 15;}

		else if((float)iterations / numIts > 0.65)
			{multHigh = multLow = 25;}

		else if((float)iterations / numIts > 0.4)
			{multHigh = multLow = 50;}

		else if((float)iterations / numIts > 0)
			{multHigh = multLow = 100;}

		if(rand() % 2 == 0)
		{
			map.insertHighArtifact(multHigh);
			contHigh++;
		}

		else
		{
			map.insertLowArtifact(multLow);
			contLow++;
		}
	}

	//* para setar todos os tiles com altura abaixo de x para y
	for(int i = 0; i < MAPSIZE; i++)
		for(int j = 0; j < MAPSIZE; j++)
		{
			if(map.Tile(j, i).getH() < MAX_H / 2)
				map.Tile(j, i).setH(0);
		}//*/

	/*// imprime erros de quando a diferença entre tiles adjacentes é maior que 1
	for(int y = 0; y < MAPSIZE; y++)
		for(int x = 0; x < MAPSIZE; x++)
		{
			for(int yOffset = -1; yOffset <= 1; yOffset++)
				for(int xOffset = -1; xOffset <= 1; xOffset++)
				{
					Pos nowPos(x, y);
					Pos adjPos(x + xOffset, y + yOffset);

					if(map.isPosInside(adjPos))
					{
						if((map.Tile(adjPos).getH() - map.Tile(nowPos).getH() > 1) || (map.Tile(adjPos).getH() - map.Tile(nowPos).getH() < -1))
						{
							printf("ERRO EM %d %d\n", x, y);
						}
					}
				}

		}//*/

	if(!SDLStart())
	{
		printf("ERRO AO INICIALIZAR SDL");

		return -1;
	}

	//Clear screen
	SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
	SDL_RenderClear(Renderer);

	int contJ = 0, contI = 0;

	//Draw vertical line of yellow dots
	for(int i = 0; i < MAPSIZE; contI++)
	{
		if(contI == MULTIPLIER_SCREENSIZE)
		{
			contI = 0;
			i++;
		}

		for(int j = 0; j < MAPSIZE; contJ++)
		{
			if(contJ == MULTIPLIER_SCREENSIZE)
			{
				contJ = 0;
				j++;
			}

/*
			if(map.Tile(j, i).getIsSeed() == true && map.Tile(j, i).seedLow == true)
				SDL_SetRenderDrawColor(Renderer, 0, 255, 0, 255);

			else if(map.Tile(j, i).getIsSeed() == true && map.Tile(j, i).seedLow == false)
				SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);

			else
			{//*/
			int hColor = map.Tile(j, i).getH();

			SDL_SetRenderDrawColor(Renderer, hColor * MULTIPLIER_COLOR, hColor * MULTIPLIER_COLOR, hColor * MULTIPLIER_COLOR, 255);
			//}

			SDL_RenderDrawPoint(Renderer, j*MULTIPLIER_SCREENSIZE + contJ, i*MULTIPLIER_SCREENSIZE + contI);
		}
	}

	//Update screen
	SDL_RenderPresent(Renderer);

	//Event handler
	SDL_Event e;

	bool quit;

	//While application is running
	while(!quit)
	{
		//Handle events on queue
		SDL_PollEvent(&e);

		if(e.type == SDL_QUIT)
			quit = true;
	}

	SDLClose();

	return 0;
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
	Window = SDL_CreateWindow("WorldGen", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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