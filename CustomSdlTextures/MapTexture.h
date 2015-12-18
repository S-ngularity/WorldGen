#ifndef MAPTEXTURE
#define MAPTEXTURE

#include "SdlClasses/SdlTexture.h"

#include <SDL2/SDL.h>

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
		std::weak_ptr<Map> worldMap;
		Uint32 *mapPixels;
		SDL_Renderer *context;

		int seaRenderMode, landRenderMode;
		
	public:
		MapTexture(SDL_Renderer *r, std::weak_ptr<Map> theMap);
		~MapTexture();

		void setMapAndUpdate(std::weak_ptr<Map> theMap);

		void update();

		int getSeaRenderMode();
		void setSeaRenderMode(int mode);

		int getLandRenderMode();
		void setLandRenderMode(int mode);
};

#endif
