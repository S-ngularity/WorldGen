#include <SDL2/SDL.h>
#include "SdlClasses/SdlTexture.h"

SdlTexture::SdlTexture()
{
	texture = NULL;
	width = 0;
	height = 0;
}

SdlTexture::SdlTexture(SDL_Texture *t, int w, int h)
{
	texture = t;
	width = w;
	height = h;
}

SdlTexture::~SdlTexture()
{
	clearTexture();
}

void SdlTexture::setTexture(SDL_Texture *t, int w, int h)
{
	clearTexture();

	texture = t;
	width = w;
	height = h;
}

void SdlTexture::clearTexture()
{
	if(texture != NULL)
	{
		SDL_DestroyTexture(texture);
		texture = NULL;		
	}	
}

void SdlTexture::render(SDL_Renderer *r, int x, int y)
{
	if(texture != NULL)
	{
		SDL_Rect renderRect = {x, y, width, height};
		SDL_RenderCopy(r, texture, NULL, &renderRect);
	}
}

void SdlTexture::renderScaled(SDL_Renderer *r, int x, int y, double sW, double sH)
{
	if(texture != NULL)
	{
		SDL_Rect renderRect = {x, y, (int)(width * sW), (int)(height * sH)};
		SDL_RenderCopy(r, texture, NULL, &renderRect);
	}
}

void SdlTexture::renderFitToArea(SDL_Renderer *r, int x, int y, int areaW, int areaH)
{
	if(texture != NULL)
	{
		double scaleW = (double)areaW / width;
		double scaleH = (double)areaH / height;

		SDL_Rect renderRect = {x, y, (int)(width * scaleW), (int)(height * scaleH)};
		SDL_RenderCopy(r, texture, NULL, &renderRect);
	}
}

int SdlTexture::getW()
{
	return width;
}

int SdlTexture::getH()
{
	return height;
}

bool SdlTexture::isNull()
{
	return texture == NULL;
}
