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

SDL_Texture* SdlTexture::getTexture()
{
	return texture;
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
		SDL_RenderCopy(r, texture, cropRect.get(), &renderRect);
	}
}

void SdlTexture::renderScaled(SDL_Renderer *r, int x, int y, double sW, double sH)
{
	if(texture != NULL)
	{
		SDL_Rect renderRect = {x, y, (int)((double)width * sW), (int)((double)height * sH)};
		SDL_RenderCopy(r, texture, cropRect.get(), &renderRect);
	}
}

void SdlTexture::renderFitToArea(SDL_Renderer *r, int x, int y, int areaW, int areaH)
{
	if(texture != NULL)
	{
		double scaleW = (double)areaW / (double)width;
		double scaleH = (double)areaH / (double)height;

		SDL_Rect renderRect = {x, y, (int)round((double)width * scaleW), (int)round((double)height * scaleH)};
		if(SDL_RenderCopy(r, texture, cropRect.get(), &renderRect) < 0)
		{
			std::cout << "SdlTexture render error: " << SDL_GetError() << std::endl;
		}
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

void SdlTexture::setColorModulation(Uint8 red, Uint8 green, Uint8 blue)
{
	SDL_SetTextureColorMod(texture, red, green, blue);
}

void SdlTexture::setBlendMode(SDL_BlendMode blending)
{
	SDL_SetTextureBlendMode(texture, blending);
}

void SdlTexture::setAlpha(Uint8 alpha)
{
	SDL_SetTextureAlphaMod(texture, alpha);
}

void SdlTexture::setCropRect(std::shared_ptr<SDL_Rect> newCropRect)
{
	cropRect = newCropRect;
}

void SdlTexture::setCropRect(int cX, int cY, int cW, int cH)
{
	if(cropRect == NULL)
		cropRect = std::make_shared<SDL_Rect>();

	cropRect->x = cX;
	cropRect->y = cY;
	cropRect->w = cW;
	cropRect->h = cH;
}

int SdlTexture::getWidth()
{
	return width;
}

int SdlTexture::getHeight()
{
	return height;
}

bool SdlTexture::isNull()
{
	return texture == NULL;
}
