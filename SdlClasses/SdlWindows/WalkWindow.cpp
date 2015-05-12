#include "SdlClasses/SdlWindows/WalkWindow.h"

#include "Map.h"

#include <stdio.h>

#include <iostream>
#include <iomanip>

#include <string>
#include <sstream>

using namespace std;

WalkWindow::WalkWindow(Map *theMap) : 
	SdlWindow(	"WorldGen Walker", 
				SDL_WINDOWPOS_CENTERED, 
				SDL_WINDOWPOS_CENTERED, 
				WALK_SCREEN_SIZE, 
				WALK_SCREEN_SIZE, 
				SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN, 
				SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE), 
	worldMap(theMap)
{
	hide(); // set superclass settings to hidden window state to sync with SDL_WINDOW_HIDDEN 

	walkX = 0; // the center tile position
	walkY = 0;

	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
	SDL_RenderSetLogicalSize(getRenderer(), WALK_SCREEN_SIZE, WALK_SCREEN_SIZE);

	//Initialize renderer color
	SDL_SetRenderDrawColor(getRenderer(), 255, 255, 255, 255);

	updateWalkTex();
	walkTexture.render(getRenderer(), 0, 0);
	refresh();
}

void WalkWindow::setPos(int x, int y)
{
	walkX = x;
	walkY = y;

	updateWalkTex();
	walkTexture.render(getRenderer(), 0, 0);
	refresh();
}

void WalkWindow::setMap(Map *m)
{
	worldMap = m;
	setPos(0, 0);
}

void WalkWindow::handleImplementedEvents(SDL_Event& e)
{
	bool updateScreen = false;

	if(hasKeyboardFocus())
	{
		switch(e.type)
		{
			case SDL_KEYDOWN:
				switch(e.key.keysym.sym)
				{
					case SDLK_UP:
						if(walkY - 1 < 0)
							walkY = 0;

						else if(worldMap->Tile(walkX, walkY - 1).getH() > worldMap->getSeaLvl())
							walkY--;
						updateScreen = true;
					break;

					case SDLK_DOWN:
						if(walkY + 1 >= worldMap->getMapHeight())
							walkY = worldMap->getMapHeight() - 1;

						else if(worldMap->Tile(walkX, walkY + 1).getH() > worldMap->getSeaLvl())
							walkY++;
						updateScreen = true;
					break;

					case SDLK_LEFT:
						if(worldMap->Tile(walkX - 1, walkY).getH() > worldMap->getSeaLvl())
						{
							if(walkX - 1 < 0)
								walkX = worldMap->getMapWidth() - 1;
							else
								walkX--;
						}

						updateScreen = true;
					break;

					case SDLK_RIGHT:
						if(worldMap->Tile(walkX + 1, walkY).getH() > worldMap->getSeaLvl())
						{
							if(walkX + 1 >= worldMap->getMapWidth())
								walkX = 0;
							
							else
								walkX++;
						}

						updateScreen = true;
					break;
				}
			break;
		}

		if(updateScreen)
		{
			updateWalkTex();
			walkTexture.render(getRenderer(), 0, 0);
			refresh();
		}
	}
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
	SDL_Texture *temp = SDL_CreateTexture(getRenderer(), 
										SDL_PIXELFORMAT_RGBA8888, 
										SDL_TEXTUREACCESS_TARGET, 
										WALK_SCREEN_SIZE,
										WALK_SCREEN_SIZE);

	SDL_SetRenderTarget(getRenderer(), temp);
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
			
			else if(worldMap->isPosInsideWrap(x, y))
			{
				if(worldMap->Tile(x, y).getH() <= worldMap->getSeaLvl())// && worldMap->getSeaLvl() >= worldMap->Tile(walkX, walkY).getH() - 2)
					SDL_SetRenderDrawColor(getRenderer(), SEA);

				else if(worldMap->Tile(x, y).getH() < worldMap->Tile(walkX, walkY).getH() - 2)
					SDL_SetRenderDrawColor(getRenderer(), DOWN); // CANT SEE DOWN

				else if(worldMap->Tile(x, y).getH() == worldMap->Tile(walkX, walkY).getH() - 2)
					SDL_SetRenderDrawColor(getRenderer(), TWODOWN); // TWO LVLS DOWN

				else if(worldMap->Tile(x, y).getH() == worldMap->Tile(walkX, walkY).getH() - 1)
					SDL_SetRenderDrawColor(getRenderer(), ONEDOWN); // ONE LVL DOWN

				else if(worldMap->Tile(x, y).getH() == worldMap->Tile(walkX, walkY).getH())
					SDL_SetRenderDrawColor(getRenderer(), GROUND); // GROUND 131, 166, 71

				else if(worldMap->Tile(x, y).getH() == worldMap->Tile(walkX, walkY).getH() + 1)
					SDL_SetRenderDrawColor(getRenderer(), ONEUP); // ONE LVL UP

				else if(worldMap->Tile(x, y).getH() == worldMap->Tile(walkX, walkY).getH() + 2)
					SDL_SetRenderDrawColor(getRenderer(), TWOUP); // ONE LVL UP

				else if(worldMap->Tile(x, y).getH() > worldMap->Tile(walkX, walkY).getH() + 2)
					SDL_SetRenderDrawColor(getRenderer(), UP); // CANT SEE UP
			}

			else
				SDL_SetRenderDrawColor(getRenderer(), 255, 255, 255, 255);

			SDL_RenderFillRect(getRenderer(), &square);
			squarePosIt++;
		}

	SDL_SetRenderTarget(getRenderer(), NULL);
	walkTexture.setTexture(temp, WALK_SCREEN_SIZE, WALK_SCREEN_SIZE);
}
