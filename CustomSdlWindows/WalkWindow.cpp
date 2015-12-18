#include "CustomSdlWindows/WalkWindow.h"

#include "Map/Map.h"

#include "Ui/UiObject.h"

#include <iostream>
#include <iomanip>

#include <string>
#include <sstream>

using namespace std;

WalkWindow::WalkWindow() : 
	SdlWindow(	"WorldGen Walker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
				WALK_SCREEN_SIZE, WALK_SCREEN_SIZE, // window size
				WALK_SCREEN_SIZE, WALK_SCREEN_SIZE, // window resolution
				SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN, 
				SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE), 
	walkTexture(std::make_shared<SdlTexture>(SDL_CreateTexture(getRenderer(), 
												SDL_PIXELFORMAT_RGBA8888, 
												SDL_TEXTUREACCESS_TARGET, 
												WALK_SCREEN_SIZE,
												WALK_SCREEN_SIZE)))
{
	hide(); // set superclass settings to hidden window state to sync with SDL_WINDOW_HIDDEN 
	
	setWindowSdlEvtHandler([&](SDL_Event &e){return customSdlEvtHandler(e);});

	// the center tile position
	walkX = 0;
	walkY = 0;

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	windowUiManager->addChild(new UiObject(0, 0, walkTexture, nullptr));
}

void WalkWindow::setPos(int x, int y)
{
	if(worldMap.expired())
		std::cout << "WalkWindow::setPos() called with an expired worldMap." << std::endl;

	//else if(!worldMap->isPosInsideNoWrap(x, y))
	//		std::cout << "WalkWindow::setPos() called with out-of-bounds x and/or y: x = " << x << " y = " << y << "." << std::endl;

	else
	{
		walkX = x;
		walkY = y;
		
		updateWalkTex();
	}
}

void WalkWindow::setMap(std::weak_ptr<Map> m)
{
	if(m.expired())
		std::cout << "WalkWindow::setMap() called with an expired argument Map *m." << std::endl;

	else
	{
		worldMap = m;
		setPos(0, 0);
	}
}

bool WalkWindow::customSdlEvtHandler(SDL_Event& e)
{
	bool returnValue = false;
	auto actualMap = worldMap.lock();

	if(actualMap == nullptr)
		std::cout << "WalkWindow::customSdlEvtHandler() called with a NULL actualMap." << std::endl;

	else if(hasKeyboardFocus())
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

						else if(actualMap->getH(walkX, walkY - 1) > actualMap->getSeaLevel())
							walkY--;
						updateScreen = true;
					break;

					case SDLK_DOWN:
					case SDLK_s:
						if(walkY + 1 >= actualMap->getMapHeight())
							walkY = actualMap->getMapHeight() - 1;

						else if(actualMap->getH(walkX, walkY + 1) > actualMap->getSeaLevel())
							walkY++;
						updateScreen = true;
					break;

					case SDLK_LEFT:
					case SDLK_a:
						if(actualMap->getH(walkX - 1, walkY) > actualMap->getSeaLevel())
						{
							if(walkX - 1 < 0)
								walkX = actualMap->getMapWidth() - 1;
							else
								walkX--;
						}

						updateScreen = true;
					break;

					case SDLK_RIGHT:
					case SDLK_d:
						if(actualMap->getH(walkX + 1, walkY) > actualMap->getSeaLevel())
						{
							if(walkX + 1 >= actualMap->getMapWidth())
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
	auto actualMap = worldMap.lock();
	if(actualMap == nullptr)
	{
		std::cout << "WalkWindow::updateWalkTex() called with a NULL actualMap." << std::endl;
		return;
	}

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
			
			else if(actualMap->isPosInsideWrap(x, y))
			{
				if(actualMap->getH(x, y) <= actualMap->getSeaLevel())// && actualMap->getSeaLevel() >= actualMap->getH(walkX, walkY) - 2)
					SDL_SetRenderDrawColor(getRenderer(), SEA);

				else if(actualMap->getH(x, y) < actualMap->getH(walkX, walkY) - 2)
					SDL_SetRenderDrawColor(getRenderer(), DOWN); // CANT SEE DOWN

				else if(actualMap->getH(x, y) == actualMap->getH(walkX, walkY) - 2)
					SDL_SetRenderDrawColor(getRenderer(), TWODOWN); // TWO LeVeLS DOWN

				else if(actualMap->getH(x, y) == actualMap->getH(walkX, walkY) - 1)
					SDL_SetRenderDrawColor(getRenderer(), ONEDOWN); // ONE LeVeL DOWN

				else if(actualMap->getH(x, y) == actualMap->getH(walkX, walkY))
					SDL_SetRenderDrawColor(getRenderer(), GROUND); // GROUND 131, 166, 71

				else if(actualMap->getH(x, y) == actualMap->getH(walkX, walkY) + 1)
					SDL_SetRenderDrawColor(getRenderer(), ONEUP); // ONE LeVeL UP

				else if(actualMap->getH(x, y) == actualMap->getH(walkX, walkY) + 2)
					SDL_SetRenderDrawColor(getRenderer(), TWOUP); // ONE LeVeL UP

				else if(actualMap->getH(x, y) > actualMap->getH(walkX, walkY) + 2)
					SDL_SetRenderDrawColor(getRenderer(), UP); // CANT SEE UP
			}

			else
				SDL_SetRenderDrawColor(getRenderer(), 255, 255, 255, 255);

			SDL_RenderFillRect(getRenderer(), &square);
			squarePosIt++;
		}

	walkTexture->releaseRenderTarget(getRenderer());
}
