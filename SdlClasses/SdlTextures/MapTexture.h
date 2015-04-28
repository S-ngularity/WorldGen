#ifndef MAPTEXTURE
#define MAPTEXTURE

#include <SDL2/SDL.h>

#include "Map.h"
#include "SdlClasses/SdlTexture.h"

// map sea & land render modes
#define NO_SEA 0
#define WITH_SEA 1
#define VARYING_HIGHEST 0
#define VARYING_MAX 1
#define FIXED 2

class MapTexture : public SdlTexture
{
	private:
		Map &map;
		Uint32 *mapPixels;
		SDL_Renderer *context;

		int seaRenderMode, landRenderMode;
		
	public:
		MapTexture(Map &theMap, SDL_Renderer *r);
		~MapTexture();

		void update();

		int getSeaRenderMode();
		void setSeaRenderMode(int mode);

		int getLandRenderMode();
		void setLandRenderMode(int mode);
};

#endif
