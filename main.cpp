#include <iostream>
#include <iomanip>

#include <string>
#include <sstream>

#include <unordered_map>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Map.h"
//#include "Noises/MyNoise.h"
#include "Noises/DiamSqNoise.h"
#include "SdlClasses/SdlTexture.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

using namespace std;

const int SCREEN_WIDTH = 800;//map.getMapWidth();
const int SCREEN_HEIGHT = 800;//map.getMapHeight();

#define walkTileNum 15
#define walkTileSize 20
const int WALK_SCREEN_SIZE = (walkTileSize * (walkTileNum*2 + 1));

// ask for noise screen update at every X percent completed
#define UPDATE_AT_PERCENT 20

// map sea & land render modes
#define NO_SEA 0
#define WITH_SEA 1
#define VARYING_HIGHEST 0
#define VARYING_MAX 1
#define FIXED 2

SDL_Window *noiseWindow = NULL, *walkWindow = NULL;
SDL_Renderer *noiseRenderer = NULL, *walkRenderer = NULL;
SDL_Event event;

TTF_Font *Font = NULL;

Map map(1025, 1025);
int seaLevel = SEA_LEVEL;
int seaRenderMode = NO_SEA, landRenderMode = FIXED;

Uint32 *mapPixels = (Uint32*) malloc(sizeof(Uint32) * map.getMapWidth() * map.getMapHeight());

DiamSqNoise noise(map);
//MyNoise noise(map);

unordered_map<string, SdlTexture*> textureMap;

//funções SDL
bool SDLStart();
void SDLClose();

void mapPosFromMouse(int *x, int *y);

void handleNoiseWidowEvent(SDL_Event windowEvent);

void createWalkWindow(int x, int y);
void destroyWalkWindow();
void handleWalkWindowEvent(SDL_Event windowEvent);
void updateWalkTex();

void updateMapTex();
void updateInfoTex();

int walkX, walkY;

bool quit = false;

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

	textureMap.emplace("heightInfo", new SdlTexture);
	textureMap.emplace("mapTexture", new SdlTexture);
	textureMap.emplace("walkTexture", new SdlTexture);
	/*textureMap.emplace("mapTexture", new SdlTexture(SDL_CreateTexture(noiseRenderer, 
																	SDL_PIXELFORMAT_RGBA8888, 
																	SDL_TEXTUREACCESS_STREAMING, 
																	map.getMapWidth(), map.getMapHeight()),
													map.getMapWidth(), map.getMapHeight())); // PARA UPDATES SEM DESTRUIR/RECRIAR?*/

	updateMapTex();
	textureMap["mapTexture"]->render(noiseRenderer, 0, 0);
	SDL_RenderPresent(noiseRenderer);

	bool updateMapTexture = false;
	int shownPercent = 0;

	while(noise.getPercentComplete() < 100 && !quit) // noise iterations
	{
		noise.runOnce();

		// update only once per percent
		if(noise.getPercentComplete() != shownPercent)
		{
			shownPercent = noise.getPercentComplete();

			cout << "\b\b\b\b" << shownPercent << "%";

			if(shownPercent == 100) // show highest at each phase
				cout << endl 
				<< endl << "Highest point: " << map.getHighestH()
				<< endl << "Lowest point: " << map.getLowestH() << endl << endl;
			
			if(shownPercent % UPDATE_AT_PERCENT == 0)
			{
				if(noise.getPercentComplete() < 100) // no sea while not done
				{
					seaRenderMode = NO_SEA;
					landRenderMode = FIXED;
					updateMapTexture = true;
				}

				else
				{
					seaRenderMode = WITH_SEA;
					landRenderMode = VARYING_HIGHEST;
					updateMapTexture = true; // last noise print
					cout << "Sea Level : " << setw(3) << setfill('0') << seaLevel;
				}
			}
		}

		if(updateMapTexture)
		{
			updateMapTex();
			textureMap["mapTexture"]->render(noiseRenderer, 0, 0);
			SDL_RenderPresent(noiseRenderer);

			updateMapTexture = false;
		}

		while(SDL_PollEvent(&event)) // handle basic events while noise is still running (closing, etc)
		{
			switch(event.type)
			{
				case SDL_QUIT:
					quit = true;
				break;
			}
		}
	}


	//While application is running
	while(!quit)
	{
		//Handle events on queue
		while(SDL_PollEvent(&event))
		{
			if(event.window.windowID == SDL_GetWindowID(noiseWindow))
				handleNoiseWidowEvent(event);

			else if(walkWindow != NULL && event.window.windowID == SDL_GetWindowID(walkWindow))
			{
				handleWalkWindowEvent(event);
			}
		}

	}

	destroyWalkWindow();

	textureMap.clear(); // destroy all textures before closing SDL systems

	SDLClose();
	free(mapPixels);

	return 0;
}

void handleNoiseWidowEvent(SDL_Event windowEvent)
{
	bool updateMapTexture = false, render = false;

	switch(windowEvent.type)
	{
		case SDL_WINDOWEVENT:
			if(windowEvent.window.event == SDL_WINDOWEVENT_CLOSE)
				quit = true;
		break;

		case SDL_KEYDOWN:
			switch(windowEvent.key.keysym.sym)
			{
				case SDLK_UP:
					// up always shows sea
					if(seaRenderMode != WITH_SEA)
					{
						seaRenderMode = WITH_SEA;
						updateMapTexture = true;
					}

					// up = +1 sea_lvl when with_sea
					else if(!(windowEvent.key.keysym.mod & KMOD_SHIFT) && seaLevel + 1 < map.getHighestH())
					{
						seaLevel += 1;
						cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << "Sea Level : " << setw(3) << setfill('0') << seaLevel;
						
						seaRenderMode = WITH_SEA;
						updateMapTexture = true;
					}
				break;

				case SDLK_DOWN:
					// shift+down = no_sea (no update when not needed)
					if(windowEvent.key.keysym.mod & KMOD_SHIFT && seaRenderMode != NO_SEA)
					{
						seaRenderMode = NO_SEA;
						landRenderMode = FIXED;
						updateMapTexture = true;
					}

					// down = -1 sealvl when with_sea
					else if(!(windowEvent.key.keysym.mod & KMOD_SHIFT) && seaLevel - 1 > 0 && seaRenderMode == WITH_SEA)
					{
						seaLevel -= 1;
						cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << "Sea Level : " << setw(3) << setfill('0') << seaLevel;

						seaRenderMode = WITH_SEA;
						updateMapTexture = true;
					}
				break;

				case SDLK_f:
					if(seaRenderMode != NO_SEA)
					{
						landRenderMode = FIXED;
						updateMapTexture = true;
					}
				break;

				case SDLK_h:
					if(seaRenderMode != NO_SEA)
					{
						landRenderMode = VARYING_HIGHEST;
						updateMapTexture = true;
					}
				break;

				case SDLK_m:
					if(seaRenderMode != NO_SEA)
					{
						landRenderMode = VARYING_MAX;
						updateMapTexture = true;
					}
				break;
			}
		break;

		case SDL_MOUSEBUTTONUP:
			if(windowEvent.button.button == SDL_BUTTON_LEFT)
			{
				int x, y;
				mapPosFromMouse(&x, &y);

				createWalkWindow(x, y);
			}
		break;

		case SDL_MOUSEMOTION:
			updateInfoTex();
			render = true;
		break;
	}

	if(updateMapTexture)
	{
		updateInfoTex(); // in case it became sea
		updateMapTex();

		render = true;
		//updateMapTexture = false;
	}

	if(render)
	{

		int x, y;
		mapPosFromMouse(&x, &y);

		//Clear screen
		//SDL_SetRenderDrawColor(noiseRenderer, 255, 255, 255, 255);
		//SDL_RenderClear(noiseRenderer);
		
		textureMap["mapTexture"]->render(noiseRenderer, 0, 0);
		textureMap["heightInfo"]->render(noiseRenderer, x, y - 30);

		SDL_RenderPresent(noiseRenderer);
		//render = false;
	}
}

void createWalkWindow(int x, int y)
{
	destroyWalkWindow();

	walkWindow = SDL_CreateWindow(	"WorldGen Walker",
									SDL_WINDOWPOS_CENTERED,
									SDL_WINDOWPOS_CENTERED,
									WALK_SCREEN_SIZE,
									WALK_SCREEN_SIZE,
									SDL_WINDOW_RESIZABLE);
	if(walkWindow == NULL)
	{
		printf("walkWindow could not be created! SDL Error: %s\n", SDL_GetError());
		return;
	}
	
	//Create renderer for window
	walkRenderer = SDL_CreateRenderer(walkWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if(walkRenderer == NULL)
	{
		printf("walkRenderer could not be created! SDL Error: %s\n", SDL_GetError());

		return;
	}

	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
	SDL_RenderSetLogicalSize(walkRenderer, WALK_SCREEN_SIZE, WALK_SCREEN_SIZE);

	//Initialize walkRenderer color
	SDL_SetRenderDrawColor(walkRenderer, 255, 255, 255, 255);


	walkX = x; // the center tile position
	walkY = y;

	updateWalkTex();
	textureMap["walkTexture"]->render(walkRenderer, 0, 0);
	SDL_RenderPresent(walkRenderer);

}

void destroyWalkWindow()
{
	if(walkWindow != NULL)
		SDL_DestroyWindow(walkWindow);

	if(walkRenderer != NULL)
		SDL_DestroyRenderer(walkRenderer);

	walkRenderer = NULL;
	walkWindow = NULL;

	textureMap["walkTexture"]->clearTexture();		// crash if SDLClose calls this function after textureMap.clear() at main
													// (but textureMap.clear() must happen before SDL systems are terminated)
}

void handleWalkWindowEvent(SDL_Event windowEvent)
{
	bool updateScreen = false;

	switch(windowEvent.type)
	{
		case SDL_WINDOWEVENT:
			if(windowEvent.window.event == SDL_WINDOWEVENT_CLOSE)
				destroyWalkWindow();
		break;

		case SDL_KEYDOWN:
			switch(windowEvent.key.keysym.sym)
			{
				case SDLK_UP:
					if(walkY - 1 < 0)
						walkY = 0;

					else if(map.Tile(walkX, walkY - 1).getH() > seaLevel)
						walkY--;
					updateScreen = true;
				break;

				case SDLK_DOWN:
					if(walkY + 1 >= map.getMapHeight())
						walkY = map.getMapHeight() - 1;

					else if(map.Tile(walkX, walkY + 1).getH() > seaLevel)
						walkY++;
					updateScreen = true;
				break;

				case SDLK_LEFT:
					if(walkX - 1 < 0)
						walkX = map.getMapWidth() - 1;

					else if(map.Tile(walkX - 1, walkY).getH() > seaLevel)
						walkX--;
					updateScreen = true;
				break;

				case SDLK_RIGHT:
					if(walkX + 1 >= map.getMapWidth())
						walkX = 0;

					else if(map.Tile(walkX + 1, walkY).getH() > seaLevel)
						walkX++;
					updateScreen = true;
				break;
			}
		break;
	}

	if(updateScreen)
	{
		updateWalkTex();
		textureMap["walkTexture"]->render(walkRenderer, 0, 0);
		SDL_RenderPresent(walkRenderer);
	}
}

#define DOWN 0, 0, 0, 255
#define TWODOWN 60, 75, 34, 255
#define ONEDOWN 85, 107, 47, 255
#define GROUND 131, 166, 71, 255
#define ONEUP 153, 204, 153, 255
#define TWOUP 172, 246, 172, 255
#define UP 198, 227, 255, 255 //30, 144, 255, 255
#define SEA 25, 45, 85, 255

void updateWalkTex()
{
	SDL_Texture *temp = SDL_CreateTexture(walkRenderer, 
										SDL_PIXELFORMAT_RGBA8888, 
										SDL_TEXTUREACCESS_TARGET, 
										WALK_SCREEN_SIZE,
										WALK_SCREEN_SIZE);

	SDL_SetRenderTarget(walkRenderer, temp);
	SDL_SetRenderDrawColor(walkRenderer, 255, 255, 255, 255);
	SDL_RenderClear(walkRenderer);

	int squarePosIt = 0;

	for(int y = walkY - walkTileNum; y <= walkY + walkTileNum; y++)
		for(int x = walkX - walkTileNum; x <= walkX + walkTileNum; x++)
		{
			SDL_Rect square = {	(squarePosIt%(walkTileNum*2 + 1)) * walkTileSize,
								(squarePosIt/(walkTileNum*2 + 1)) * walkTileSize,
								(squarePosIt%(walkTileNum*2 + 1)) * walkTileSize + walkTileSize,
								(squarePosIt/(walkTileNum*2 + 1)) * walkTileSize + walkTileSize};

			if(x == walkX && y == walkY)
				SDL_SetRenderDrawColor(walkRenderer, 255, 0, 0, 255);
			
			else if(map.isPosInsideWrap(x, y))
			{
				if(map.Tile(x, y).getH() <= seaLevel)// && seaLevel >= map.Tile(walkX, walkY).getH() - 2)
					SDL_SetRenderDrawColor(walkRenderer, SEA);

				else if(map.Tile(x, y).getH() < map.Tile(walkX, walkY).getH() - 2)
					SDL_SetRenderDrawColor(walkRenderer, DOWN); // CANT SEE DOWN

				else if(map.Tile(x, y).getH() == map.Tile(walkX, walkY).getH() - 2)
					SDL_SetRenderDrawColor(walkRenderer, TWODOWN); // TWO LVLS DOWN

				else if(map.Tile(x, y).getH() == map.Tile(walkX, walkY).getH() - 1)
					SDL_SetRenderDrawColor(walkRenderer, ONEDOWN); // ONE LVL DOWN

				else if(map.Tile(x, y).getH() == map.Tile(walkX, walkY).getH())
					SDL_SetRenderDrawColor(walkRenderer, GROUND); // GROUND 131, 166, 71

				else if(map.Tile(x, y).getH() == map.Tile(walkX, walkY).getH() + 1)
					SDL_SetRenderDrawColor(walkRenderer, ONEUP); // ONE LVL UP

				else if(map.Tile(x, y).getH() == map.Tile(walkX, walkY).getH() + 2)
					SDL_SetRenderDrawColor(walkRenderer, TWOUP); // ONE LVL UP

				else if(map.Tile(x, y).getH() > map.Tile(walkX, walkY).getH() + 2)
					SDL_SetRenderDrawColor(walkRenderer, UP); // CANT SEE UP
			}

			else
				SDL_SetRenderDrawColor(walkRenderer, 255, 255, 255, 255);

			SDL_RenderFillRect(walkRenderer, &square);
			squarePosIt++;
		}

	SDL_SetRenderTarget(walkRenderer, NULL);
	textureMap["walkTexture"]->setTexture(temp, WALK_SCREEN_SIZE, WALK_SCREEN_SIZE);
}

void updateMapTex()
{
	SDL_Texture *temp = SDL_CreateTexture(noiseRenderer, 
										SDL_PIXELFORMAT_RGBA8888, 
										SDL_TEXTUREACCESS_STREAMING, 
										map.getMapWidth(), map.getMapHeight());

	Uint32 *pixelIt = mapPixels;
	Uint8 r, g, b, a = 255;

	for(int y = 0; y < map.getMapHeight(); y++)
		for(int x = 0; x < map.getMapWidth(); x++)
		{
			if(map.Tile(x, y).getError() == true)
			{
				r = 100;
				g = 0;
				b = 0;
			}
			/*
			else if(map.Tile(x, y).getIsSeed() == true)// && map.Tile(x, y).seedLow == true)
				SDL_SetRenderDrawColor(noiseRenderer, 0, 255, 0, 255);
			//*/

			else if(seaRenderMode == WITH_SEA && map.Tile(x, y).getH() <= seaLevel)
			{
				r = 25;
				g = 45;
				b = 85;
			}

			else
			{
				Uint8 baseColor, hColor;
				
				if(landRenderMode == VARYING_HIGHEST) // BRANCO VARIAVEL seaLevel até HighestH
				{
					baseColor = 100;
					float multiplierColor = (float)(255 - baseColor) / (map.getHighestH() - seaLevel);
					
					hColor = (map.Tile(x, y).getH() - seaLevel) * multiplierColor;
				}//*/
				//*
				else if(landRenderMode == VARYING_MAX) // BRANCO VARIAVEL seaLevel até MAX_H
				{
					baseColor = 100;
					float multiplierColor = (float)(255 - baseColor) / (MAX_H - seaLevel);
					
					hColor = (map.Tile(x, y).getH() - seaLevel) * multiplierColor;
				}//*/

				else if(landRenderMode == FIXED) // BRANCO FIXO
				{
					baseColor = 0;
					float multiplierColor = (float)(255 - baseColor) / MAX_H;

					hColor = map.Tile(x, y).getH() * multiplierColor;
				}

				r = baseColor + hColor;
				g = baseColor + hColor;
				b = baseColor + hColor;
			}

			*pixelIt = r << 24 | g << 16 | b << 8 | a;

			pixelIt++;
		}

	SDL_UpdateTexture(temp, NULL, mapPixels, map.getMapWidth() * sizeof (Uint32));
	textureMap["mapTexture"]->setTexture(temp, map.getMapWidth(), map.getMapHeight());
}

void updateInfoTex()
{
	int x, y;
	mapPosFromMouse(&x, &y);

	int h = map.Tile(x, y).getH();
	string info;

	if(seaRenderMode == WITH_SEA && h <= seaLevel)
		info = "Sea";

	else
	{
		stringstream ss;
		ss << h;
		info = ss.str();
	}

	SDL_Surface* tempSurface = TTF_RenderText_Blended(Font, info.c_str(), {220,20,60});
	if(tempSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		return;
	}

	else
	{
		//Create texture from surface pixels
		SDL_Texture *tempTex = SDL_CreateTextureFromSurface(noiseRenderer, tempSurface); // SDL_TEXTUREACCESS_STATIC
		if(tempTex == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
			return;
		}

		textureMap["heightInfo"]->setTexture(tempTex, tempSurface->w, tempSurface->h);

		SDL_FreeSurface(tempSurface);
    }
}

void mapPosFromMouse(int *x, int *y)
{
	SDL_GetMouseState(x, y);

	int winW, winH;
	SDL_GetWindowSize(noiseWindow, &winW, &winH);

	float scaleX, scaleY;
	SDL_RenderGetScale(noiseRenderer, &scaleX, &scaleY);

	// regra de três: mouseX / winW assim como mapX / map.getMapWidth()
	int sobra; // letterbox, soma dos 2 lados
	if((float)map.getMapWidth() / winW - (float)map.getMapHeight() / winH > 0) // ratioW - ratioH (ratio do original em relação a janela)
	{
		sobra = (winH - map.getMapHeight() * scaleY); // total - tamanho do mapa(renderer logical size) * escala que esta sendo renderizado
		
		*x = (*x * map.getMapWidth())/(float)winW;
		if(*y < sobra/2) // está no letterbox antes
			*y = 0;

		else if(*y > map.getMapHeight() * scaleY + sobra/2) // está no letterbox depois
			*y = map.getMapHeight() - 1;

		else // escala y sem letterbox em window size sem letterbox em relação ao mapa
			*y = ((*y - sobra/2) * map.getMapHeight())/(float)(winH - sobra);
	}

	else
	{
		sobra = (winW - map.getMapWidth() * scaleX);

		*y = (*y * map.getMapHeight())/(float)winH;

		if(*x < sobra/2)
			*x = 0;

		else if(*x >  map.getMapWidth() * scaleX + sobra/2)
			*x = map.getMapWidth() - 1;

		else
			*x = ((*x - sobra/2) * map.getMapWidth())/(float)(winW - sobra);
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

	//Initialize SDL_ttf
	if(TTF_Init() == -1)
	{
		printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}

	Font = TTF_OpenFont("Resources/OpenSans-Regular.ttf", 20);
	if(Font == NULL)
	{
		printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}
	TTF_SetFontStyle(Font, TTF_STYLE_BOLD);

	//Create window
	noiseWindow = SDL_CreateWindow("WorldGen", 20, 40, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
	if(noiseWindow == NULL)
	{
		printf("noiseWindow could not be created! SDL Error: %s\n", SDL_GetError());

		return false;
	}
	
	//Create renderer for window
	noiseRenderer = SDL_CreateRenderer(noiseWindow, -1, SDL_RENDERER_ACCELERATED);
	if(noiseRenderer == NULL)
	{
		printf("noiseRenderer could not be created! SDL Error: %s\n", SDL_GetError());

		return false;
	}

	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
	SDL_RenderSetLogicalSize(noiseRenderer, map.getMapWidth(), map.getMapHeight());

	//Initialize noiseRenderer color
	SDL_SetRenderDrawColor(noiseRenderer, 0, 0, 0, 0);

	return true;
}

void SDLClose()
{
	//Destroy window	
	SDL_DestroyRenderer(noiseRenderer);
	SDL_DestroyWindow(noiseWindow);
	noiseWindow = NULL;
	noiseRenderer = NULL;

	TTF_Quit();
	//Quit SDL subsystems
	SDL_Quit();
}