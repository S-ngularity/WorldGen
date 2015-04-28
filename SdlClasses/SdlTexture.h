#ifndef SDLTEXTURE
#define SDLTEXTURE

#include <SDL2/SDL.h>

class SdlTexture
{
	private:
		SDL_Texture *texture;

		int width;
		int height;

	public:
		SdlTexture();
		SdlTexture(SDL_Texture *t, int w, int h);
		~SdlTexture();
		
		void render(SDL_Renderer *r, int x, int y);

		void setTexture(SDL_Texture *t, int width, int height);
		void clearTexture();
		
		int getW();
		int getH();
};

#endif
