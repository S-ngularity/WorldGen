#ifndef SDLTEXTURE
#define SDLTEXTURE

#include <SDL2/SDL.h>

class SdlTexture
{
	public:
		SdlTexture();
		SdlTexture(SDL_Texture *t, int w, int h);
		~SdlTexture();

		void setTexture(SDL_Texture *t, int width, int height);
		void clearTexture();
		
		void render(SDL_Renderer *r, int x, int y);

		int getW();
		int getH();

	private:
		SDL_Texture *texture;

		int width;
		int height;
};

#endif