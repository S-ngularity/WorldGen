#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "map.h"

#include <SDL2/SDL.h>

using namespace std;

const int SCREEN_WIDTH = MAPSIZE * 3;
const int SCREEN_HEIGHT = MAPSIZE * 3;

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
	cout << endl;

	for(int iterations = 0; iterations < numIts; iterations++)
	{
		if(rand() % 2 == 0)
			map.insertHighArtifact();

		else
			map.insertLowArtifact();
	}

	/* para setar todos os tiles com altura abaixo de x para y
	for(int i = 0; i < MAPSIZE; i++)
		for(int j = 0; j < MAPSIZE; j++)
		{
			if(map.Tile(j, i).getH() < 5)
				map.Tile(j, i).setH(0);
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
		if(contI == 3)
		{
			contI = 0;
			i++;
		}

		for(int j = 0; j < MAPSIZE; contJ++)
		{
			if(contJ == 3)
			{
				contJ = 0;
				j++;
			}

			int hColor = map.Tile(j, i).getH();

			SDL_SetRenderDrawColor(Renderer, hColor * 25, hColor * 25, hColor * 25, 255);
			SDL_RenderDrawPoint(Renderer, j*3 + contJ, i*3 + contI);
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

	//*// imprime erros de quando a diferença entre tiles adjacentes é maior que 1
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

	//if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	//{
	//	printf("Warning: Linear texture filtering not enabled!");
	//}

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