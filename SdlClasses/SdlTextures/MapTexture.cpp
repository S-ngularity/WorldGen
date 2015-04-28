#include "SdlClasses/SdlTextures/MapTexture.h"

#include <stdlib.h>

MapTexture::MapTexture(Map *theMap, SDL_Renderer *r)
{
	worldMap = theMap;
	context = r;
	mapPixels = (Uint32*) malloc(sizeof(Uint32) * worldMap->getMapWidth() * worldMap->getMapHeight());
	seaRenderMode = NO_SEA;
	landRenderMode = FIXED;
}

MapTexture::~MapTexture()
{
	free(mapPixels);
}

void MapTexture::setMap(Map *theMap)
{
	int w = worldMap->getMapWidth();
	int h = worldMap->getMapHeight();

	worldMap = theMap;

	if(worldMap->getMapWidth() != w || worldMap->getMapHeight() != h)
	{
		delete mapPixels;
		mapPixels = (Uint32*) malloc(sizeof(Uint32) * worldMap->getMapWidth() * worldMap->getMapHeight());
	}
}

void MapTexture::setContext(SDL_Renderer *r)
{
	context = r;
}

void MapTexture::update()
{
	SDL_Texture *temp = SDL_CreateTexture(context, 
										SDL_PIXELFORMAT_RGBA8888, 
										SDL_TEXTUREACCESS_STREAMING, 
										worldMap->getMapWidth(), worldMap->getMapHeight());

	Uint32 *pixelIt = mapPixels;
	Uint8 r, g, b, a = 255;

	for(int y = 0; y < worldMap->getMapHeight(); y++)
		for(int x = 0; x < worldMap->getMapWidth(); x++)
		{
			if(worldMap->Tile(x, y).getError() == true)
			{
				r = 100;
				g = 0;
				b = 0;
			}
			/*
			else if(worldMap->Tile(x, y).getIsSeed() == true)// && worldMap->Tile(x, y).seedLow == true)
				SDL_SetRenderDrawColor(noiseRenderer, 0, 255, 0, 255);
			//*/

			else if(seaRenderMode == WITH_SEA && worldMap->Tile(x, y).getH() <= worldMap->getSeaLvl())
			{
				r = 25;
				g = 45;
				b = 85;
			}

			else
			{
				Uint8 baseColor, hColor;
				
				if(landRenderMode == VARYING_HIGHEST) // BRANCO VARIAVEL worldMap->getSeaLvl() até HighestH
				{
					baseColor = 100;
					int varBy = (worldMap->getHighestH() - worldMap->getSeaLvl());
					if(varBy == 0) varBy = 1;
					float multiplierColor = (float)(255 - baseColor) / varBy;
					
					hColor = (worldMap->Tile(x, y).getH() - worldMap->getSeaLvl()) * multiplierColor;
				}//*/
				//*
				else if(landRenderMode == VARYING_MAX) // BRANCO VARIAVEL worldMap->getSeaLvl() até MAX_H
				{
					baseColor = 100;
					int varBy = (MAX_H - worldMap->getSeaLvl());
					if(varBy == 0) varBy = 1;
					float multiplierColor = (float)(255 - baseColor) / varBy;
					
					hColor = (worldMap->Tile(x, y).getH() - worldMap->getSeaLvl()) * multiplierColor;
				}//*/

				else if(landRenderMode == FIXED) // BRANCO FIXO
				{
					baseColor = 0;
					int varBy = worldMap->getHighestH();
					if(varBy == 0) varBy = 1;
					float multiplierColor = (float)(255 - baseColor) / varBy;

					hColor = worldMap->Tile(x, y).getH() * multiplierColor;
				}

				r = baseColor + hColor;
				g = baseColor + hColor;
				b = baseColor + hColor;
			}

			*pixelIt = r << 24 | g << 16 | b << 8 | a;

			pixelIt++;
		}

	SDL_UpdateTexture(temp, NULL, mapPixels, worldMap->getMapWidth() * sizeof (Uint32));
	setTexture(temp, worldMap->getMapWidth(), worldMap->getMapHeight());
}


int MapTexture::getSeaRenderMode()
{
	return seaRenderMode;
}

void MapTexture::setSeaRenderMode(int mode)
{
	if(mode == NO_SEA)
		seaRenderMode = NO_SEA;

	else if(mode == WITH_SEA)
		seaRenderMode = WITH_SEA;
}

int MapTexture::getLandRenderMode()
{
	return landRenderMode;
}

void MapTexture::setLandRenderMode(int mode)
{
	if(mode == VARYING_HIGHEST)
		landRenderMode = VARYING_HIGHEST;

	else if(mode == VARYING_MAX)
		landRenderMode = VARYING_MAX;

	else if(mode == FIXED)
		landRenderMode = FIXED;
}
