#ifndef SDLTEXTURE
#define SDLTEXTURE

#include <stack>

#include <SDL2/SDL.h>

class SdlTexture
{
	private:
		static std::stack<SdlTexture*> renderTargetStack;

		SDL_Texture *texture;

		int width;
		int height;

	public:
		SdlTexture();
		SdlTexture(SDL_Texture *t);
		SdlTexture(SDL_Texture *t, int w, int h);
		virtual ~SdlTexture();
		
		void render(SDL_Renderer *r, int x, int y);
		void renderScaled(SDL_Renderer *r, int x, int y, double sW, double sH);
		void renderFitToArea(SDL_Renderer *r, int x, int y, int areaW, int areaH);

		void setAsRenderTarget(SDL_Renderer *r);
		void releaseRenderTarget(SDL_Renderer *r);

		void setTexture(SDL_Texture *t);
		void setTexture(SDL_Texture *t, int width, int height);
		void clearTexture();
		
		int getW();
		int getH();

		bool isNull();
};

#endif
