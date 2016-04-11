#ifndef MAPTEXTURE
#define MAPTEXTURE

#include "SdlClasses/SdlTexture.h"

#include <SDL.h>

class Map;

// map sea & land render modes
#define NO_SEA 0
#define WITH_SEA 1
#define VARYING_HIGHEST 0
#define VARYING_MAX 1
#define FIXED 2

class MapTexture : public SdlTexture
{
	private:
		Uint32 *mapPixels;
		SDL_Renderer *rendererContext;

		int seaRenderMode, landRenderMode;
		
	public:
		MapTexture(SDL_Renderer *r, const Map &theMap);
		MapTexture(const MapTexture&) = delete;
		MapTexture& operator=(const MapTexture&) = delete;
		~MapTexture();

		void update(const Map &theMap);

		int getSeaRenderMode() const;
		void setSeaRenderMode(int mode);

		int getLandRenderMode() const;
		void setLandRenderMode(int mode);
};

#endif
