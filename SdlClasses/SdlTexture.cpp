#include <SDL2/SDL.h>
#include "SdlClasses/SdlTexture.h"

#include <cmath>
#include <iostream>

std::stack<SdlTexture*> SdlTexture::renderTargetStack;

SdlTexture::SdlTexture()
{
	texture = NULL;
	width = 0;
	height = 0;
}

SdlTexture::SdlTexture(SDL_Texture *t)
{
	texture = t;
	SDL_QueryTexture(texture, NULL, NULL, &width, &height);
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

void SdlTexture::setTexture(SDL_Texture *t)
{
	clearTexture();

	texture = t;
	SDL_QueryTexture(texture, NULL, NULL, &width, &height);
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
		SDL_Rect renderRect = {x, y, (int)((double)width * sW), (int)((double)height * sH)};
		SDL_RenderCopy(r, texture, NULL, &renderRect);
	}
}

void SdlTexture::renderFitToArea(SDL_Renderer *r, int x, int y, int areaW, int areaH)
{
	if(texture != NULL)
	{
		double scaleW = (double)areaW / (double)width;
		double scaleH = (double)areaH / (double)height;

		SDL_Rect renderRect = {x, y, (int)round((double)width * scaleW), (int)round((double)height * scaleH)};
		SDL_RenderCopy(r, texture, NULL, &renderRect);
	}
}

void SdlTexture::setAsRenderTarget(SDL_Renderer *r)
{
	renderTargetStack.push(this);

	SDL_SetRenderTarget(r, texture);
}

void SdlTexture::releaseRenderTarget(SDL_Renderer *r)
{
	if(renderTargetStack.empty())
		std::cout << "Error on releasing Render Target from SdlTexture: no current SdlTexture as render target." << std::endl;

	else if(renderTargetStack.top() != this)
		std::cout << "Error on releasing Render Target from SdlTexture: current render target is not this SdlTexture." << std::endl;

	else
	{
		renderTargetStack.pop();

		if(!renderTargetStack.empty())
			SDL_SetRenderTarget(r, renderTargetStack.top()->texture);
		else
			SDL_SetRenderTarget(r, NULL);
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
