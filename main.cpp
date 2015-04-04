#include <iostream>
#include <iomanip>

#include <string>
#include <sstream>

#include <unordered_map>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Map.h"
#include "MyNoise.h"
#include "SdlClasses/SdlTexture.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

using namespace std;

const int SCREEN_WIDTH = MAPWIDTH;
const int SCREEN_HEIGHT = MAPHEIGHT;

// ask for update at every X percent completed
#define UPDATE_AT_PERCENT 20

// map sea & land render modes
#define NO_SEA 0
#define WITH_SEA 1
#define VARYING_HIGHEST 0
#define VARYING_MAX 1
#define FIXED 2

SDL_Window *Window = NULL;
SDL_Renderer *Renderer = NULL;
SDL_Event event;

TTF_Font *Font = NULL;

Uint32 *mapPixels = (Uint32*) malloc(sizeof(Uint32) * MAPWIDTH * MAPHEIGHT);

unordered_map<string, SdlTexture*> textureMap;

//funções SDL
bool SDLStart();
void SDLClose();

void updateMapTex();
void updateInfoTex(int x, int y);

Map map;
MyNoise noise(map);

int seaLevel = SEA_LEVEL;
int seaRenderMode = NO_SEA, landRenderMode = FIXED;

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
	/*textureMap.emplace("mapTexture", new SdlTexture(SDL_CreateTexture(Renderer, 
																	SDL_PIXELFORMAT_RGBA8888, 
																	SDL_TEXTUREACCESS_STREAMING, 
																	SCREEN_WIDTH, SCREEN_HEIGHT),
													SCREEN_WIDTH, SCREEN_HEIGHT));*/

	updateMapTex();
	textureMap["mapTexture"]->render(Renderer, 0, 0);
	SDL_RenderPresent(Renderer);

	bool alreadyUpdated = false;
	int shownPercent = 0;
	bool updateTexture = false, render = false;
	bool quit;

	//While application is running
	while(!quit)
	{
		if(!noise.isDone()) // noise iterations
		{
			noise.runOnce();

			if(noise.getPercentComplete() != shownPercent)
			{
				shownPercent = noise.getPercentComplete();

				cout << "\b\b\b\b" << shownPercent << "%";

				if(shownPercent == 100) // show highest at each phase
					cout << endl << endl << "Highest point: " << noise.getHighestH() << endl << endl;

				// update only once per percent
				bool shouldUpdate = shownPercent % UPDATE_AT_PERCENT == 0;
				if(shouldUpdate && !alreadyUpdated)
				{
					alreadyUpdated = true;

					if(!noise.isDone()) // no sea while not done
					{
						seaRenderMode = NO_SEA;
						landRenderMode = FIXED;
						updateTexture = true;
					}

					else
					{
						seaRenderMode = WITH_SEA;
						landRenderMode = VARYING_HIGHEST;
						updateTexture = true; // last noise print
						cout << "Sea Level : " << setw(3) << setfill('0') << SEA_LEVEL;
					}
				}

				else if (!shouldUpdate)
					alreadyUpdated = false;
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
							// up always shows sea
							if(seaRenderMode != WITH_SEA)
							{
								seaRenderMode = WITH_SEA;
								updateTexture = true;
							}

							// up = +1 sea_lvl when with_sea
							else if(!(event.key.keysym.mod & KMOD_SHIFT) && seaLevel + 1 < noise.getHighestH())
							{
								seaLevel += 1;
								cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << "Sea Level : " << setw(3) << setfill('0') << seaLevel;
								
								seaRenderMode = WITH_SEA;
								updateTexture = true;
							}
						break;

						case SDLK_DOWN:
							// shift+down = no_sea (no update when not needed)
							if(event.key.keysym.mod & KMOD_SHIFT && seaRenderMode != NO_SEA)
							{
								seaRenderMode = NO_SEA;
								landRenderMode = FIXED;
								updateTexture = true;
							}

							// down = -1 sealvl when with_sea
							else if(!(event.key.keysym.mod & KMOD_SHIFT) && seaLevel - 1 > 0 && seaRenderMode == WITH_SEA)
							{
								seaLevel -= 1;
								cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << "Sea Level : " << setw(3) << setfill('0') << seaLevel;

								seaRenderMode = WITH_SEA;
								updateTexture = true;
							}
						break;

						case SDLK_f:
							if(seaRenderMode != NO_SEA)
							{
								landRenderMode = FIXED;
								updateTexture = true;
							}
						break;

						case SDLK_h:
							if(seaRenderMode != NO_SEA)
							{
								landRenderMode = VARYING_HIGHEST;
								updateTexture = true;
							}
						break;

						case SDLK_m:
							if(seaRenderMode != NO_SEA)
							{
								landRenderMode = VARYING_MAX;
								updateTexture = true;
							}
						break;
					}
				break;

				case SDL_MOUSEMOTION:
					int x, y;
					SDL_GetMouseState(&x, &y);
					updateInfoTex(x, y);
					render = true;
				break;
			}
		}

		if(updateTexture)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);
			updateInfoTex(x, y);
			
			updateMapTex();

			render = true;
			updateTexture = false;
		}

		if(render)
		{

			int x, y;
			SDL_GetMouseState(&x, &y);

			//Clear screen
			//SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
			//SDL_RenderClear(Renderer);
			
			textureMap["mapTexture"]->render(Renderer, 0, 0);
			textureMap["heightInfo"]->render(Renderer, x, y + 15);

			SDL_RenderPresent(Renderer);

			render = false;
		}
	}

	textureMap.clear();

	SDLClose();
	free(mapPixels);

	return 0;
}

void updateMapTex()
{
	SDL_Texture *temp = SDL_CreateTexture(Renderer, 
										SDL_PIXELFORMAT_RGBA8888, 
										SDL_TEXTUREACCESS_STREAMING, 
										SCREEN_WIDTH, SCREEN_HEIGHT);

	Uint32 *pixelIt = mapPixels;
	Uint8 r, g, b, a = 255;

	for(int y = 0; y < MAPHEIGHT; y++)
		for(int x = 0; x < MAPWIDTH; x++)
		{
			if(map.Tile(x, y).getError() == true)
			{
				r = 100;
				g = 0;
				b = 0;
			}
			/*
			else if(map.Tile(x, y).getIsSeed() == true)// && map.Tile(x, y).seedLow == true)
				SDL_SetRenderDrawColor(Renderer, 0, 255, 0, 255);
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
					float multiplierColor = (float)(255 - baseColor) / (noise.getHighestH() - seaLevel);
					
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

	SDL_UpdateTexture(temp, NULL, mapPixels, MAPWIDTH * sizeof (Uint32));
	textureMap["mapTexture"]->setTexture(temp, MAPWIDTH, MAPHEIGHT);
}

void updateInfoTex(int x, int y)
{
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
		SDL_Texture *tempTex = SDL_CreateTextureFromSurface(Renderer, tempSurface);
		if(tempTex == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
			return;
		}

		textureMap["heightInfo"]->setTexture(tempTex, tempSurface->w, tempSurface->h);

		SDL_FreeSurface(tempSurface);
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

	TTF_Quit();
	//Quit SDL subsystems
	SDL_Quit();
}