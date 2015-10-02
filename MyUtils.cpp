#include "MyUtils.h"

#include <iostream>

SDL_Texture* MyUtils::loadTexture(SDL_Renderer *r, std::string path)
{
	// The final texture
	SDL_Texture* newTexture = NULL;

	// Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	
	if(loadedSurface == NULL)
	{
		std::cout << "Unable to load image " << path.c_str() << "! SDL_image error: " << IMG_GetError() << std::endl;
		return NULL;
	}

	SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 255, 0, 255));

	// Create texture from surface pixels
	newTexture = SDL_CreateTextureFromSurface(r, loadedSurface); // SDL_TEXTUREACCESS_STATIC
	
	if(newTexture == NULL)
		std::cout << "Unable to create texture from " << path.c_str() << "! SDL error: " << SDL_GetError() << std::endl;

	// Get rid of old loaded surface
	SDL_FreeSurface(loadedSurface);

	// Standard blend mode
	//SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND);

	return newTexture;
}
