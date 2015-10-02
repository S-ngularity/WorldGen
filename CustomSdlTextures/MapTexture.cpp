#include "Map/Map.h"
#include "CustomSdlTextures/MapTexture.h"

MapTexture::MapTexture(SDL_Renderer *r, Map *theMap)
{
	context = r;
	worldMap = theMap;
	mapPixels = new Uint32[worldMap->getMapWidth() * worldMap->getMapHeight()];
	seaRenderMode = NO_SEA;
	landRenderMode = FIXED;

	SDL_Texture *temp = SDL_CreateTexture(context, 
											SDL_PIXELFORMAT_RGBA8888, 
											SDL_TEXTUREACCESS_STREAMING, 
											worldMap->getMapWidth(), worldMap->getMapHeight());

	setTexture(temp, worldMap->getMapWidth(), worldMap->getMapHeight());

	update();
}

MapTexture::~MapTexture()
{
	delete mapPixels;
}

void MapTexture::setMapAndUpdate(Map *theMap)
{
	int w = worldMap->getMapWidth();
	int h = worldMap->getMapHeight();

	worldMap = theMap;

	if(worldMap->getMapWidth() != w || worldMap->getMapHeight() != h)
	{
		delete mapPixels;
		mapPixels = new Uint32[worldMap->getMapWidth() * worldMap->getMapHeight()];

		SDL_Texture *temp = SDL_CreateTexture(context, 
											SDL_PIXELFORMAT_RGBA8888, 
											SDL_TEXTUREACCESS_STREAMING, 
											worldMap->getMapWidth(), worldMap->getMapHeight());

		setTexture(temp, worldMap->getMapWidth(), worldMap->getMapHeight());
	}

	update();
}

void MapTexture::update()
{
	Uint32 *pixelIt = mapPixels;
	Uint8 r, g, b, a = 255;

	for(int y = 0; y < worldMap->getMapHeight(); y++)
		for(int x = 0; x < worldMap->getMapWidth(); x++)
		{
			if(seaRenderMode == WITH_SEA && worldMap->getH(x, y) <= worldMap->getSeaLevel())
			{
				r = 25;
				g = 45;
				b = 85;
			}

			else
			{
				Uint8 baseColor, hColor;
				
				if(landRenderMode == VARYING_HIGHEST) // BRANCO VARIAVEL worldMap->getSeaLevel() até HighestH
				{
					baseColor = 100;
					
					int varBy = (worldMap->getHighestH() - worldMap->getSeaLevel());
					if(varBy == 0) varBy = 1;
					
					float multiplierColor = (float)(255 - baseColor) / varBy;
					
					hColor = (worldMap->getH(x, y) - worldMap->getSeaLevel()) * multiplierColor;
				}

				else if(landRenderMode == VARYING_MAX) // BRANCO VARIAVEL worldMap->getSeaLevel() até MAX_H
				{
					baseColor = 100;
					
					int varBy = (MAX_H - worldMap->getSeaLevel());
					if(varBy == 0) varBy = 1;
					
					float multiplierColor = (float)(255 - baseColor) / varBy;
					
					hColor = (worldMap->getH(x, y) - worldMap->getSeaLevel()) * multiplierColor;
				}

				else if(landRenderMode == FIXED) // BRANCO FIXO
				{
					baseColor = 0;
					
					int varBy = MAX_H; //worldMap->getHighestH();
					if(varBy == 0) varBy = 1;;
					
					float multiplierColor = (float)(255 - baseColor) / varBy;

					hColor = worldMap->getH(x, y) * multiplierColor;
				}

				r = baseColor + hColor;
				g = baseColor + hColor;
				b = baseColor + hColor;
			}

			*pixelIt = r << 24 | g << 16 | b << 8 | a;

			pixelIt++;
		}


	void *texturePixels;
	int texturePitch;

	SDL_LockTexture(getTexture(), NULL, &texturePixels, &texturePitch);
	memcpy(texturePixels, mapPixels, worldMap->getMapWidth() * sizeof (Uint32) * worldMap->getMapHeight());
	SDL_UnlockTexture(getTexture());
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
