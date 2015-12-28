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

	if(texture != NULL)
		SDL_QueryTexture(texture, NULL, NULL, &width, &height);

	else
	{
		width = 0;
		height = 0;

		std::cout << "SdlTexture constructed with a NULL pointer passed as argument. If the intention was to construct a null texture, use SdlTexture::SdlTexture() (no arguments)." << std::endl;
	}
}

SdlTexture::SdlTexture(SDL_Texture *t, int w, int h)
{
	texture = t;
	width = w;
	height = h;

	if(texture == NULL)
		std::cout << "SdlTexture constructed with a NULL pointer passed as argument. If the intention was to construct a null texture, use SdlTexture::SdlTexture() (no arguments)." << std::endl;
}

SdlTexture::~SdlTexture()
{
	clearTexture();
}

SDL_Texture* SdlTexture::getSDL_Texture()
{
	return texture;
}

void SdlTexture::setTexture(SDL_Texture *t)
{
	clearTexture();

	texture = t;

	if(texture != NULL)
		SDL_QueryTexture(texture, NULL, NULL, &width, &height);

	else
	{
		width = 0;
		height = 0;

		std::cout << "SdlTexture::setTexture() called on a SdlTexture with a NULL argument SDL_Texture *t. If the intention was to clear the texture, use SdlTexture::clearTexture()." << std::endl;
	}
}


void SdlTexture::setTexture(SDL_Texture *t, int w, int h)
{
	clearTexture();

	texture = t;
	width = w;
	height = h;

	if(texture == NULL)
		std::cout << "SdlTexture::setTexture() called on a SdlTexture with a NULL argument SDL_Texture *t.  If the intention was to clear the texture, use SdlTexture::clearTexture()." << std::endl;
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
	if(r == NULL)
		std::cout << "SdlTexture::render() called with a NULL argument SDL_Renderer *r." << std::endl;

	else  if(texture == NULL)
		std::cout << "SdlTexture::render() called on a SdlTexture with a NULL texture." << std::endl;

	else
	{
		SDL_Rect renderRect = {x, y, width, height};
		
		if(SDL_RenderCopy(r, texture, cropRect.get(), &renderRect) < 0)
			std::cout << "SdlTexture render error: " << SDL_GetError() << std::endl;
	}

}

void SdlTexture::renderScaled(SDL_Renderer *r, int x, int y, double sW, double sH)
{
	if(r == NULL)
		std::cout << "SdlTexture::renderScaled() called with a NULL argument SDL_Renderer *r." << std::endl;

	else  if(texture == NULL)
		std::cout << "SdlTexture::renderScaled() called on a SdlTexture with a NULL texture." << std::endl;

	else
	{
		SDL_Rect renderRect = {x, y, (int)((double)width * sW), (int)((double)height * sH)};
		
		if(SDL_RenderCopy(r, texture, cropRect.get(), &renderRect) < 0)
			std::cout << "SdlTexture render error: " << SDL_GetError() << std::endl;
	}
}

void SdlTexture::renderFitToArea(SDL_Renderer *r, int x, int y, int areaW, int areaH)
{
	if(r == NULL)
		std::cout << "SdlTexture::renderFitToArea() called with a NULL argument SDL_Renderer *r." << std::endl;

	else  if(texture == NULL)
		std::cout << "SdlTexture::renderFitToArea() called on a SdlTexture with a NULL texture." << std::endl;

	else
	{
		double scaleW = (double)areaW / (double)width;
		double scaleH = (double)areaH / (double)height;

		SDL_Rect renderRect = {x, y, (int)round((double)width * scaleW), (int)round((double)height * scaleH)};
		
		if(SDL_RenderCopy(r, texture, cropRect.get(), &renderRect) < 0)
			std::cout << "SdlTexture render error: " << SDL_GetError() << std::endl;
	}
}

void SdlTexture::setAsRenderTarget(SDL_Renderer *r)
{
	if(r == NULL)
		std::cout << "SdlTexture::setAsRenderTarget() called with a NULL argument SDL_Renderer *r." << std::endl;

	if(texture == NULL)
		std::cout << "SdlTexture::setAsRenderTarget() called on a SdlTexture with a NULL texture." << std::endl;

	renderTargetStack.push(this);

	SDL_SetRenderTarget(r, texture);
}

void SdlTexture::releaseRenderTarget(SDL_Renderer *r)
{
	if(r == NULL)
		std::cout << "SdlTexture::releaseAsRenderTarget() called with a NULL argument SDL_Renderer *r." << std::endl;

	if(renderTargetStack.empty())
		std::cout << "Error on releasing Render Target from SdlTexture: no current SdlTexture as render target." << std::endl;

	else if(renderTargetStack.top() != this)
		std::cout << "Error on releasing Render Target from SdlTexture: current render target is not this SdlTexture." << std::endl;

	else
	{
		renderTargetStack.pop();

		if(!renderTargetStack.empty())
		{
			SDL_SetRenderTarget(r, renderTargetStack.top()->texture);

			if(texture == NULL)
				std::cout << "SdlTexture::releaseAsRenderTarget() tried to set the RenderTarget to the next SdlTexture on the stack, but it's texture was NULL." << std::endl;
		}

		else
			SDL_SetRenderTarget(r, NULL);
	}
}

void SdlTexture::setColorModulation(Uint8 red, Uint8 green, Uint8 blue)
{
	if(texture == NULL)
		std::cout << "SdlTexture::setColorModulation() called on a SdlTexture with a NULL texture." << std::endl;

	else
		SDL_SetTextureColorMod(texture, red, green, blue);
}

void SdlTexture::setBlendMode(SDL_BlendMode blending)
{
	if(texture == NULL)
		std::cout << "SdlTexture::setBlendMode() called on a SdlTexture with a NULL texture." << std::endl;

	else
		SDL_SetTextureBlendMode(texture, blending);
}

void SdlTexture::setAlpha(Uint8 alpha)
{
	if(texture == NULL)
		std::cout << "SdlTexture::setAlpha() called on a SdlTexture with a NULL texture." << std::endl;

	else
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

int SdlTexture::getWidth() const
{
	return width;
}

int SdlTexture::getHeight() const
{
	return height;
}

bool SdlTexture::isNull() const
{
	return texture == NULL;
}
