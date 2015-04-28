#ifndef WALKWINDOW
#define WALKWINDOW

#include "SdlClasses/SdlWindow.h"
#include "SdlClasses/SdlTexture.h"
#include "SdlClasses/SdlTextures/MapTexture.h"
#include "Map.h"

//#include "../Noises/MyNoise.h"
//#include "../Noises/DiamSqNoise.h"
#include "Noises/OpenSimplexNoise.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define walkTileNum 15
#define walkTileSize 20
const int WALK_SCREEN_SIZE = (walkTileSize * (walkTileNum*2 + 1));

class WalkWindow : public SdlWindow
{
	private:
		Map *worldMap;
		SdlTexture walkTexture;
		
		int walkX, walkY;

		void updateWalkTex();

	public:
		WalkWindow(Map *theMap);
		
		void setPos(int x, int y);
		void setMap(Map *m);

		void handleImplementedEvents(SDL_Event& e);
};

#endif
