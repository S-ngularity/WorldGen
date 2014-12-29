#include <iostream>
#include <iomanip>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Map.h"
#include "MyNoise.h"

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

void renderMap();
void renderMapNoSea();

Map map;
MyNoise noise(map);

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
	int asking;

	//While application is running
	while(!quit)
	{
		noise.runOnce();

		highestH = noise.getHighestH();

		asking = noise.askingForScreenUpdate();

		if(asking == WITHOUT_SEA)
			renderMapNoSea();

		else if(asking == WITH_SEA)
			renderMap();
		
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

	SDLClose();

	return 0;
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
			else if(map.Tile(x, y).getIsSeed() == true)// && map.Tile(x, y).seedLow == true)
				SDL_SetRenderDrawColor(Renderer, 0, 255, 0, 255);
			//*/

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
			else if(map.Tile(x, y).getIsSeed() == true)// && map.Tile(x, y).seedLow == true)
				SDL_SetRenderDrawColor(Renderer, 0, 255, 0, 255);
			//*/

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