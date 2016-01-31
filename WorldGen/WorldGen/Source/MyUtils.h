#ifndef MYUTILS
#define MYUTILS

#include <memory>
#include <iostream>

#include <SDL.h>
#include <SDL_image.h>

class SdlTexture;

class MyUtils
{
	public:
		static std::shared_ptr<SdlTexture> loadTexture(SDL_Renderer *r, std::string path);
};

#endif