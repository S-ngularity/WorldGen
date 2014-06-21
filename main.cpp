#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "map.h"

#include <SDL2/SDL.h>

using namespace std;

#define MULTIPLIER_SCREENSIZE 4
#define MULTIPLIER_COLOR 255 / MAX_H

const int SCREEN_WIDTH = MAPWIDTH * MULTIPLIER_SCREENSIZE;
const int SCREEN_HEIGHT = MAPHEIGHT * MULTIPLIER_SCREENSIZE;

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

	float mult;

	for(int iterations = 0; iterations < numIts; iterations++)
	{
		if((float)iterations / numIts > 0.8)
			mult = 15;

		else if((float)iterations / numIts > 0.7)
			mult = 25;

		else if((float)iterations / numIts > 0.6)
			mult = 50;

		else
			mult = 100;


		if(rand() % 2 == 0)
			map.insertHighArtifact(mult);

		else
			map.insertLowArtifact(mult);
	}
int highestH = 0, lowestH = MAX_H;
	//* para setar todos os tiles com altura abaixo de x para y
	for(int y = 0; y < MAPHEIGHT; y++)
		for(int x = 0; x < MAPWIDTH; x++)
		{
			if(map.Tile(x, y).getH() > highestH)
				highestH = map.Tile(x, y).getH();

			if(map.Tile(x, y).getH() < lowestH)
				lowestH = map.Tile(x, y).getH();

			if(map.Tile(x, y).getH() < MAX_H / 2)
				map.Tile(x, y).setH(0);
		}//*/
printf("highestH = %d\n", highestH);
printf("lowestH = %d\n", lowestH);
	/*// imprime erros de quando a diferença entre tiles adjacentes é maior que 1
	for(int y = 0; y < MAPHEIGHT; y++)
		for(int x = 0; x < MAPWIDTH; x++)
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

	int contX = 0, contY = 0;

	//Draw vertical line of yellow dots
	for(int y = 0; y < MAPHEIGHT; contY++)
	{
		if(contY == MULTIPLIER_SCREENSIZE)
		{
			contY = 0;
			y++;
		}

		for(int x = 0; x < MAPWIDTH; contX++)
		{
			if(contX == MULTIPLIER_SCREENSIZE)
			{
				contX = 0;
				x++;
			}

/*
			if(map.Tile(x, y).getIsSeed() == true && map.Tile(x, y).seedLow == true)
				SDL_SetRenderDrawColor(Renderer, 0, 255, 0, 255);

			else if(map.Tile(x, y).getIsSeed() == true && map.Tile(x, y).seedLow == false)
				SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);

			else
			{//*/
			int hColor = map.Tile(x, y).getH();

			SDL_SetRenderDrawColor(Renderer, hColor * MULTIPLIER_COLOR, hColor * MULTIPLIER_COLOR, hColor * MULTIPLIER_COLOR, 255);
			//}

			SDL_RenderDrawPoint(Renderer, x * MULTIPLIER_SCREENSIZE + contX, y * MULTIPLIER_SCREENSIZE + contY);
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