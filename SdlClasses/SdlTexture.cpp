#include <SDL2/SDL.h>
#include "SdlTexture.h"

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
		SDL_DestroyTexture(texture);	
}

void SdlTexture::render(SDL_Renderer *r, int x, int y)
{
	if(texture != NULL)
	{
		SDL_Rect renderRect = {x, y, width, height};
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