#include "Map/Map.h"
#include "CustomSdlTextures/MapTexture.h"

#include <iostream>

MapTexture::MapTexture(SDL_Renderer *r, std::weak_ptr<Map> theMap)
{
	context = r;
	worldMap = theMap;
	seaRenderMode = NO_SEA;
	landRenderMode = FIXED;

	auto actualMap = worldMap.lock();
	if(actualMap == nullptr)
		std::cout << "MapTexture::MapTexture() called with a NULL actualMap." << std::endl;

	else
	{
		mapPixels = new Uint32[actualMap->getMapWidth() * actualMap->getMapHeight()];

		SDL_Texture *temp = SDL_CreateTexture(context, 
												SDL_PIXELFORMAT_RGBA8888, 
												SDL_TEXTUREACCESS_STREAMING, 
												actualMap->getMapWidth(), actualMap->getMapHeight());

		setTexture(temp, actualMap->getMapWidth(), actualMap->getMapHeight());

		update();
	}
}

MapTexture::~MapTexture()
{
	delete mapPixels;
}

void MapTexture::setMapAndUpdate(std::weak_ptr<Map> theMap)
{
	auto actualMap = worldMap.lock();
	if(actualMap == nullptr)
		std::cout << "MapTexture::setMapAndUpdate() called with a NULL actualMap." << std::endl;

	int oldW = actualMap->getMapWidth();
	int oldH = actualMap->getMapHeight();

	worldMap = theMap;

	actualMap = worldMap.lock();
	if(actualMap == nullptr)
		std::cout << "MapTexture::setMapAndUpdate() called with a NULL actualMap (from argument theMap)." << std::endl;

	if(actualMap->getMapWidth() != oldW || actualMap->getMapHeight() != oldH)
	{
		delete mapPixels;
		mapPixels = new Uint32[actualMap->getMapWidth() * actualMap->getMapHeight()];

		SDL_Texture *temp = SDL_CreateTexture(context, 
											SDL_PIXELFORMAT_RGBA8888, 
											SDL_TEXTUREACCESS_STREAMING, 
											actualMap->getMapWidth(), actualMap->getMapHeight());

		setTexture(temp, actualMap->getMapWidth(), actualMap->getMapHeight());
	}

	update();
}

void MapTexture::update()
{
	auto actualMap = worldMap.lock();
	if(actualMap == nullptr)
		std::cout << "MapTexture::update() called with a NULL actualMap." << std::endl;

	Uint32 *pixelIt = mapPixels;
	Uint8 r, g, b, a = 255;
	
	int dashedLineCounter = -1;

	for(int y = 0; y < actualMap->getMapHeight(); y++)
	{
		// Count pixels of the dashed line
		++dashedLineCounter;
		if(dashedLineCounter == 30)
			dashedLineCounter = 0;

		for(int x = 0; x < actualMap->getMapWidth(); x++)
		{
			// Color map limits
			if(x == 0 && (y == 0 || y == actualMap->getMapHeight()-1))
			{
				r = 0;
				g = 255;
				b = 0;
			}

			// dashed vertical line
			else if(x == 0 && dashedLineCounter < 10)
			{
				r = 75;
				g = 110;
				b = 190;
			}

			// Sea
			else if(seaRenderMode == WITH_SEA && actualMap->getH(x, y) <= actualMap->getSeaLevel())
			{
				r = 25;
				g = 45;
				b = 85;
			}

			// Land
			else
			{
				Uint8 baseColor, hColor;
				
				if(landRenderMode == VARYING_HIGHEST) // BRANCO VARIAVEL actualMap->getSeaLevel() até HighestH
				{
					baseColor = 100;
					
					int varBy = (actualMap->getHighestH() - actualMap->getSeaLevel());
					if(varBy == 0) varBy = 1;
					
					float multiplierColor = (float)(255 - baseColor) / varBy;
					
					hColor = (actualMap->getH(x, y) - actualMap->getSeaLevel()) * multiplierColor;
				}

				else if(landRenderMode == VARYING_MAX) // BRANCO VARIAVEL actualMap->getSeaLevel() até MAX_H
				{
					baseColor = 100;
					
					int varBy = (MAX_H - actualMap->getSeaLevel());
					if(varBy == 0) varBy = 1;
					
					float multiplierColor = (float)(255 - baseColor) / varBy;
					
					hColor = (actualMap->getH(x, y) - actualMap->getSeaLevel()) * multiplierColor;
				}

				else if(landRenderMode == FIXED) // BRANCO FIXO
				{
					baseColor = 0;
					
					int varBy = MAX_H; //actualMap->getHighestH();
					if(varBy == 0) varBy = 1;;
					
					float multiplierColor = (float)(255 - baseColor) / varBy;

					hColor = actualMap->getH(x, y) * multiplierColor;
				}

				r = baseColor + hColor;
				g = baseColor + hColor;
				b = baseColor + hColor;
			}

			*pixelIt = r << 24 | g << 16 | b << 8 | a;

			pixelIt++;
		}
	}

	void *texturePixels;
	int texturePitch;

	SDL_LockTexture(getSDL_Texture(), NULL, &texturePixels, &texturePitch);
	memcpy(texturePixels, mapPixels, actualMap->getMapWidth() * sizeof (Uint32) * actualMap->getMapHeight());
	SDL_UnlockTexture(getSDL_Texture());
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
