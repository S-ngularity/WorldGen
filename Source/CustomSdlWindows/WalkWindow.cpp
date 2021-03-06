#include "CustomSdlWindows/WalkWindow.h"

#include "Ui/UiObject.h"

#include <iostream>
#include <iomanip>

#include <string>
#include <sstream>

using namespace std;

WalkWindow::WalkWindow(Map theMap, int x, int y) : 
	SdlWindow(	"WorldGen Walker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
				WALK_SCREEN_SIZE, WALK_SCREEN_SIZE, // window size
				WALK_SCREEN_SIZE, WALK_SCREEN_SIZE, // window resolution
				SDL_WINDOW_RESIZABLE,  
				SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE), 
	worldMap(theMap), 
	walkTexture(std::make_shared<SdlTexture>(SDL_CreateTexture(getRenderer(), 
												SDL_PIXELFORMAT_ARGB8888, 
												SDL_TEXTUREACCESS_TARGET, 
												WALK_SCREEN_SIZE,
												WALK_SCREEN_SIZE)))
{
	setWindowSdlEvtHandler([&](const SDL_Event &e){return customSdlEvtHandler(e);});

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	windowUiManager->addChild(std::make_shared<UiObject>(0, 0, walkTexture->getWidth(), walkTexture->getHeight(), walkTexture, nullptr));

	// if(!worldMap.isPosInsideNoWrap(x, y))
	// {
	// 	std::cout << "WalkWindow::setPos() called with out-of-bounds x and/or y: x = " << x << " y = " << y << "." << std::endl;
	// 	return;
	// }

	walkX = x;
	walkY = y;
	
	updateWalkTex();
}

void WalkWindow::setPos(int x, int y)
{
	// if(!worldMap.isPosInsideNoWrap(x, y))
	// {
	// 	std::cout << "WalkWindow::setPos() called with out-of-bounds x and/or y: x = " << x << " y = " << y << "." << std::endl;
	// 	return;
	// }

	
	walkX = x;
	walkY = y;
	
	updateWalkTex();
}

bool WalkWindow::customSdlEvtHandler(const SDL_Event &e)
{
	bool returnValue = false;

	if(hasKeyboardFocus())
	{
		bool updateScreen = false;
		
		returnValue = true;

		switch(e.type)
		{
			case SDL_KEYDOWN:
				switch(e.key.keysym.sym)
				{
					case SDLK_UP:
					case SDLK_w:
						if(walkY - 1 < 0)
							walkY = 0;

						else if(worldMap.getH(walkX, walkY - 1) > worldMap.getSeaLevel())
							walkY--;
						updateScreen = true;
					break;

					case SDLK_DOWN:
					case SDLK_s:
						if(walkY + 1 >= worldMap.getMapHeight())
							walkY = worldMap.getMapHeight() - 1;

						else if(worldMap.getH(walkX, walkY + 1) > worldMap.getSeaLevel())
							walkY++;
						updateScreen = true;
					break;

					case SDLK_LEFT:
					case SDLK_a:
						if(worldMap.getH(walkX - 1, walkY) > worldMap.getSeaLevel())
						{
							if(walkX - 1 < 0)
								walkX = worldMap.getMapWidth() - 1;
							else
								walkX--;
						}

						updateScreen = true;
					break;

					case SDLK_RIGHT:
					case SDLK_d:
						if(worldMap.getH(walkX + 1, walkY) > worldMap.getSeaLevel())
						{
							if(walkX + 1 >= worldMap.getMapWidth())
								walkX = 0;
							
							else
								walkX++;
						}

						updateScreen = true;
					break;

					default:
						returnValue = false;
					break;
				}
			break;

			default:
				returnValue = false;
			break;
		}

		if(updateScreen)
		{
			updateWalkTex();
		}
	}

	return returnValue;
}

#define DOWN 0, 0, 0, 255
#define TWODOWN 60, 75, 34, 255
#define ONEDOWN 85, 107, 47, 255
#define GROUND 131, 166, 71, 255
#define ONEUP 153, 204, 153, 255
#define TWOUP 172, 246, 172, 255
#define UP 198, 227, 255, 255 //30, 144, 255, 255
#define SEA 25, 45, 85, 255

void WalkWindow::updateWalkTex()
{
	walkTexture->setAsRenderTarget(getRenderer());
	SDL_SetRenderDrawColor(getRenderer(), 0, 0, 0, 255);
	SDL_RenderClear(getRenderer());

	int squarePosIt = 0;

	for(int y = walkY - walkTileNum; y <= walkY + walkTileNum; y++)
		for(int x = walkX - walkTileNum; x <= walkX + walkTileNum; x++)
		{
			SDL_Rect square = {	(squarePosIt%(walkTileNum*2 + 1)) * walkTileSize,
								(squarePosIt/(walkTileNum*2 + 1)) * walkTileSize,
								(squarePosIt%(walkTileNum*2 + 1)) * walkTileSize + walkTileSize,
								(squarePosIt/(walkTileNum*2 + 1)) * walkTileSize + walkTileSize};

			if(x == walkX && y == walkY)
				SDL_SetRenderDrawColor(getRenderer(), 255, 0, 0, 255);
			
			else if(worldMap.isPosInsideWrap(x, y))
			{
				if(worldMap.getH(x, y) <= worldMap.getSeaLevel())// && worldMap.getSeaLevel() >= worldMap.getH(walkX, walkY) - 2)
					SDL_SetRenderDrawColor(getRenderer(), SEA);

				else if(worldMap.getH(x, y) < worldMap.getH(walkX, walkY) - 2)
					SDL_SetRenderDrawColor(getRenderer(), DOWN); // CANT SEE DOWN

				else if(worldMap.getH(x, y) == worldMap.getH(walkX, walkY) - 2)
					SDL_SetRenderDrawColor(getRenderer(), TWODOWN); // TWO LeVeLS DOWN

				else if(worldMap.getH(x, y) == worldMap.getH(walkX, walkY) - 1)
					SDL_SetRenderDrawColor(getRenderer(), ONEDOWN); // ONE LeVeL DOWN

				else if(worldMap.getH(x, y) == worldMap.getH(walkX, walkY))
					SDL_SetRenderDrawColor(getRenderer(), GROUND); // GROUND 131, 166, 71

				else if(worldMap.getH(x, y) == worldMap.getH(walkX, walkY) + 1)
					SDL_SetRenderDrawColor(getRenderer(), ONEUP); // ONE LeVeL UP

				else if(worldMap.getH(x, y) == worldMap.getH(walkX, walkY) + 2)
					SDL_SetRenderDrawColor(getRenderer(), TWOUP); // ONE LeVeL UP

				else if(worldMap.getH(x, y) > worldMap.getH(walkX, walkY) + 2)
					SDL_SetRenderDrawColor(getRenderer(), UP); // CANT SEE UP
			}

			else
				SDL_SetRenderDrawColor(getRenderer(), 255, 255, 255, 255);

			SDL_RenderFillRect(getRenderer(), &square);
			squarePosIt++;
		}

	walkTexture->releaseRenderTarget(getRenderer());
}
