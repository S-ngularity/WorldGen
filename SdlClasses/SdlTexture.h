#ifndef SDLTEXTURE
#define SDLTEXTURE

#include <stack>
#include <memory>

#include <SDL2/SDL.h>

class SdlTexture
{
	private:
		static std::stack<SdlTexture*> renderTargetStack;

		SDL_Texture *texture;
		std::shared_ptr<SDL_Rect> cropRect;

		int width;
		int height;

	protected:
		SDL_Texture* getSDL_Texture();

	public:
		SdlTexture();
		SdlTexture(SDL_Texture *t);
		SdlTexture(SDL_Texture *t, int w, int h);
		SdlTexture(const SdlTexture&) = delete;
		SdlTexture& operator=(const SdlTexture&) = delete;
		virtual ~SdlTexture();
		
		void render(SDL_Renderer *r, int x, int y);
		void renderScaled(SDL_Renderer *r, int x, int y, double sW, double sH);
		void renderFitToArea(SDL_Renderer *r, int x, int y, int areaW, int areaH);

		void setAsRenderTarget(SDL_Renderer *r);
		void releaseRenderTarget(SDL_Renderer *r);

		void setTexture(SDL_Texture *t);
		void setTexture(SDL_Texture *t, int width, int height);
		void clearTexture();

		void setColorModulation(Uint8 red, Uint8 green, Uint8 blue);
		void setBlendMode(SDL_BlendMode blending);
		void setAlpha(Uint8 alpha);

		void setCropRect(std::shared_ptr<SDL_Rect> newCropRect);
		void setCropRect(int cX, int cY, int cW, int cH);
		
		int getWidth() const;
		int getHeight() const;

		bool isNull() const;
};

#endif
