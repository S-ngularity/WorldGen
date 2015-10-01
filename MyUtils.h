#ifndef MYUTILS
#define MYUTILS

#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class MyUtils
{
	public:
		static SDL_Texture* loadTexture(SDL_Renderer *r, std::string path);
};

#endif