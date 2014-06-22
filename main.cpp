#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "map.h"

#include <SDL2/SDL.h>

using namespace std;

#define MULTIPLIER_SCREENSIZE 1

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

	int percentComplete = 0;
	float floatComplete = 0;

	int state = 0, lastState = -1;
	int multValue = 100;
	Pos seedPos;

	cout << "0%";

	for(int iterations = 0; iterations < numIts; iterations++)
	{
		floatComplete = (float)iterations / (float)numIts;

		if(floatComplete < 0.05)
		{
			state = 0;
			multValue = 100;
		}

		else if(floatComplete < 0.25)
		{
			//lastState = 0;
			state = 1;
			multValue = 100;
		}

		else if(floatComplete < 0.35)
		{
			state = 2;
			multValue = 30;
		}

		else if(floatComplete < 0.75)
		{
			state = 3;
			multValue = 20;
		}

		else
		{
			state = 4;
			multValue = 10;
		}

		/*
		if(state > lastState + 1) // zera todos os tiles com altura abaixo de X quando passa de estado
		{
			lastState = state - 1;

			for(int y = 0; y < MAPHEIGHT; y++)
				for(int x = 0; x < MAPWIDTH; x++)
					if(map.Tile(x, y).getH() < SEA)
						map.Tile(x, y).setH(0);
		}//*/

		if(state == 0)
		{
			seedPos.setPos((rand() % MAPWIDTH), (rand() % MAPHEIGHT));
			map.insertHighArtifact(seedPos, multValue);
		}

		else if(state == 1)
		{
			if(rand() % 2 == 0)
			{
				seedPos.setPos((rand() % MAPWIDTH), (rand() % MAPHEIGHT));
				map.insertHighArtifact(seedPos, multValue);
			}

			else
			{
				do{
					seedPos.setPos((rand() % MAPWIDTH), (rand() % MAPHEIGHT));
				}while(map.Tile(seedPos.getX(), seedPos.getY()).getH() <= SEA);

				map.insertLowArtifact(seedPos, multValue);
			}
		}

		else
		{
			do{
				seedPos.setPos((rand() % MAPWIDTH), (rand() % MAPHEIGHT));
			}while(map.Tile(seedPos.getX(), seedPos.getY()).getH() <= SEA);

			if(rand() % 2 == 0)
				map.insertHighArtifact(seedPos, multValue);

			else
				map.insertLowArtifact(seedPos, multValue);
		}


		if((int)(100 * floatComplete) > percentComplete)
		{
			percentComplete = (int)(100 * floatComplete);

			cout << "\b\b\b" << percentComplete << "%";
		}
	}

	cout << "\b\b\b" << "100%" << endl << endl;

	int highestH = 0;
	
	for(int y = 0; y < MAPHEIGHT; y++)
		for(int x = 0; x < MAPWIDTH; x++)
			if(map.Tile(x, y).getH() > highestH)
				highestH = map.Tile(x, y).getH();

	cout << "highest tile = " << highestH << endl;
	cout << "sea = " << SEA << endl;

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

	int baseColor = 100;
	int MULTIPLIER_COLOR = (255 - baseColor) / (highestH - SEA);

	//Draw vertical line of yellow dots
	for(int y = 0; y < MAPHEIGHT; y++)
	{

		for(int x = 0; x < MAPWIDTH; x++)
		{/*
			if(map.Tile(x, y).getIsSeed() == true && map.Tile(x, y).seedLow == true)
				SDL_SetRenderDrawColor(Renderer, 0, 255, 0, 255);

			else if(map.Tile(x, y).getIsSeed() == true && map.Tile(x, y).seedLow == false)
				SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);

			else
			{//*/

			if(map.Tile(x, y).getH() <= SEA)
				SDL_SetRenderDrawColor(Renderer, 25, 45, 85, 255);//SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);//
			else
			{
				int hColor = (map.Tile(x, y).getH() - SEA) * MULTIPLIER_COLOR;

				SDL_SetRenderDrawColor(Renderer, baseColor + hColor, baseColor + hColor, baseColor + hColor, 255);
			}

			SDL_RenderDrawPoint(Renderer, x, y);
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

		SDL_RenderPresent(Renderer);
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