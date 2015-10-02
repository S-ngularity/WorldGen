#include "MyUtils.h"

#include <iostream>

SDL_Texture* MyUtils::loadTexture(SDL_Renderer *r, std::string path)
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	
	if(loadedSurface == NULL)
	{
		std::cout << "Unable to load image " << path.c_str() << "! SDL_image error: " << IMG_GetError() << std::endl;
		return NULL;
	}

	//Create texture from surface pixels
	newTexture = SDL_CreateTextureFromSurface(r, loadedSurface);
	
	if(newTexture == NULL)
		std::cout << "Unable to create texture from " << path.c_str() << "! SDL error: " << SDL_GetError() << std::endl;

	//Get rid of old loaded surface
	SDL_FreeSurface(loadedSurface);

	return newTexture;
}
