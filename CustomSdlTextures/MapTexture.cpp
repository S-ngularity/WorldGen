#include "Map/Map.h"
#include "CustomSdlTextures/MapTexture.h"

#include <iostream>

MapTexture::MapTexture(SDL_Renderer *r, Map &theMap) : 
	rendererContext(r), 
	seaRenderMode(NO_SEA), 
	landRenderMode(FIXED)
{
	if(r == NULL)
		std::cout << "MapTexture::MapTexture() called with a NULL argument SDL_Renderer *r." << std::endl;

	mapPixels = new Uint32[theMap.getMapWidth() * theMap.getMapHeight()];

	SDL_Texture *temp = SDL_CreateTexture(rendererContext, 
											SDL_PIXELFORMAT_RGBA8888, 
											SDL_TEXTUREACCESS_STREAMING, 
											theMap.getMapWidth(), theMap.getMapHeight());

	setTexture(temp, theMap.getMapWidth(), theMap.getMapHeight());

	update(theMap);
}

MapTexture::~MapTexture()
{
	delete mapPixels;
}

void MapTexture::update(Map &theMap)
{
	if(theMap.getMapWidth() != getWidth() || theMap.getMapHeight() != getHeight())
	{
		delete mapPixels;
		mapPixels = new Uint32[theMap.getMapWidth() * theMap.getMapHeight()];

		SDL_Texture *temp = SDL_CreateTexture(rendererContext, 
											SDL_PIXELFORMAT_RGBA8888, 
											SDL_TEXTUREACCESS_STREAMING, 
											theMap.getMapWidth(), theMap.getMapHeight());

		setTexture(temp, theMap.getMapWidth(), theMap.getMapHeight());
	}

	Uint32 *pixelIt = mapPixels;
	Uint8 r, g, b, a = 255;
	
	int dashedLineCounter = -1;

	for(int y = 0; y < theMap.getMapHeight(); y++)
	{
		// Count pixels of the dashed line
		++dashedLineCounter;
		if(dashedLineCounter == 30)
			dashedLineCounter = 0;

		for(int x = 0; x < theMap.getMapWidth(); x++)
		{
			// Color map limits
			if(x == 0 && (y == 0 || y == theMap.getMapHeight()-1))
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
			else if(seaRenderMode == WITH_SEA && theMap.getH(x, y) <= theMap.getSeaLevel())
			{
				r = 25;
				g = 45;
				b = 85;
			}

			// Land
			else
			{
				Uint8 baseColor, hColor;
				
				if(landRenderMode == VARYING_HIGHEST) // BRANCO VARIAVEL theMap.getSeaLevel() até HighestH
				{
					baseColor = 100;
					
					int varBy = (theMap.getHighestH() - theMap.getSeaLevel());
					if(varBy == 0) varBy = 1;
					
					float multiplierColor = (float)(255 - baseColor) / varBy;
					
					hColor = (theMap.getH(x, y) - theMap.getSeaLevel()) * multiplierColor;
				}

				else if(landRenderMode == VARYING_MAX) // BRANCO VARIAVEL theMap.getSeaLevel() até MAX_H
				{
					baseColor = 100;
					
					int varBy = (MAX_H - theMap.getSeaLevel());
					if(varBy == 0) varBy = 1;
					
					float multiplierColor = (float)(255 - baseColor) / varBy;
					
					hColor = (theMap.getH(x, y) - theMap.getSeaLevel()) * multiplierColor;
				}

				else if(landRenderMode == FIXED) // BRANCO FIXO
				{
					baseColor = 0;
					
					int varBy = MAX_H; //theMap.getHighestH();
					if(varBy == 0) varBy = 1;;
					
					float multiplierColor = (float)(255 - baseColor) / varBy;

					hColor = theMap.getH(x, y) * multiplierColor;
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
	memcpy(texturePixels, mapPixels, theMap.getMapWidth() * sizeof (Uint32) * theMap.getMapHeight());
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
