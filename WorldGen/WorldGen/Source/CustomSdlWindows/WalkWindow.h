#ifndef WALKWINDOW
#define WALKWINDOW

#include "Map/Map.h"
#include "SdlClasses/SdlWindow.h"
#include "SdlClasses/SdlTexture.h"

#include <memory>

#include <SDL.h>

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

		bool customSdlEvtHandler(const SDL_Event &e);

	public:
		WalkWindow(Map theMap, int x, int y);
		WalkWindow(const WalkWindow&) = delete;
		WalkWindow& operator=(const WalkWindow&) = delete;
		~WalkWindow() = default;
		
		void setPos(int x, int y);
};

#endif
