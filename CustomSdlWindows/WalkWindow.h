#ifndef WALKWINDOW
#define WALKWINDOW

#include "Map/Map.h"
#include "SdlClasses/SdlWindow.h"
#include "SdlClasses/SdlTexture.h"

#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define walkTileNum 15
#define walkTileSize 20
const int WALK_SCREEN_SIZE = (walkTileSize * (walkTileNum*2 + 1));

class WalkWindow : public SdlWindow
{
	private:
		Map worldMap;

		std::shared_ptr<SdlTexture> walkTexture;
		
		int walkX, walkY;

		void updateWalkTex();

		bool customSdlEvtHandler(SDL_Event& e);

	public:
		WalkWindow(Map theMap, int x, int y);
		
		void setPos(int x, int y);
};

#endif
