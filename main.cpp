#include <iostream>
#include <iomanip>

#include <unordered_map>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Map.h"
#include "SdlClasses/SdlWindows/NoiseWindow.h"
//#include "SdlClasses/SdlWindows/WalkWindow.h"

#include <SDL2/SDL.h>

using namespace std;

SDL_Event event;

NoiseWindow *noiseWindow;

const int mapNum = 3;

Map worldMap(1025, 1025);
Map worldMap2(1025, 1025);
Map worldMap3(1025, 1025);
Map* worldMapsVect[mapNum] = {&worldMap, &worldMap2, &worldMap3};

//funções SDL
bool SDLStart();
void SDLClose();

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
		printf("ERRO AO INICIALIZAR SDL\n");

		return -1;
	}

	noiseWindow = new NoiseWindow(worldMapsVect, mapNum);

	// while window is open
	while(noiseWindow->isShown())
	{
		while(SDL_PollEvent(&event))
		{
			noiseWindow->handleEvent(event);
		}
	}

	delete noiseWindow;

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

	//Initialize SDL_ttf
	if(TTF_Init() == -1)
	{
		printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}

	return true;
}

void SDLClose()
{
	TTF_Quit();
	//Quit SDL subsystems
	SDL_Quit();
}