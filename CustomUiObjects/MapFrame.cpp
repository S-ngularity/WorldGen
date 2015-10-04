#include "CustomUiObjects/MapFrame.h"

#include "Map/Map.h"
//#include "Map/Noises/MyNoise.h"
#include "Map/Noises/DiamSqNoise.h"
#include "Map/Noises/OpenSimplexNoise.h"

#include "CustomSdlTextures/MapTexture.h"

#include "Ui/UiManager.h"
#include "DefaultUiObjects/UiLabel.h"

#include "Ui/EventAggregator.h"
#include "Ui/UiEvents/UiEventCode.h"
#include "Ui/UiEvents/WalkWindowOpened.h"
#include "Ui/UiEvents/MapInfoUpdate.h"

#include <iostream>
#include <iomanip>

#include <string>
#include <sstream>

MapFrame::MapFrame(UiManager *parentUiMngr, int x, int y, int w, int h, Map* mapArr[], int num) : 
	UiObject(x, y, w, h, NULL,
			[&](SDL_Event &e){return customSdlEvtHandler(e);})
{
	mapArray = mapArr;
	numMaps = num;
	selectedMap = 0;

	noiseArray[0] = new OpenSimplexNoise(mapArray[0], octaves, freq, persistence, freqDiv);
	noiseArray[1] = new DiamSqNoise(mapArray[0]);
	selectedNoise = 0;

	setParentUiManager(parentUiMngr);

	if(parentUiManager == NULL)
		std::cout << "MapFrame without parentUiManager (is NULL)." << std::endl;

	mapTexture = std::make_shared<MapTexture>(parentUiManager->getRenderer(), mapArray[selectedMap]);
	setUiObjectTexture(mapTexture);

	mouseTooltip = new UiLabel(0, 0, ALIGN_BOTTOM_LEFT, "", 20, 220, 20, 60);
	addChild(mouseTooltip);

	publishMapInfo();
}

MapFrame::~MapFrame()
{
	for(int i = 0; i < 3; i++)
		delete noiseArray[i];
}

void MapFrame::runNoise()
{
	bool updateMapTexture = false;
	int shownPercent = 0, updateAt = 0;

	noiseArray[selectedNoise]->setMap(mapArray[selectedMap]);

	mapTexture->setSeaRenderMode(NO_SEA);  // no sea while not done
	mapTexture->setLandRenderMode(FIXED);

	while(noiseArray[selectedNoise]->getPercentComplete() < 100) // noise iterations
	{
		noiseArray[selectedNoise]->runOnce();

		// update only once per percent
		if(noiseArray[selectedNoise]->getPercentComplete() != shownPercent)
		{
			shownPercent = noiseArray[selectedNoise]->getPercentComplete();

			if(shownPercent >= updateAt || shownPercent >= 100)
			{
				updateAt += UPDATE_AT_PERCENT;
				updateMapTexture = true;

				if(shownPercent >= 100)
				{
					mapTexture->setSeaRenderMode(WITH_SEA);
					mapTexture->setLandRenderMode(VARYING_HIGHEST);

					mapArray[selectedMap]->setSeaLevel(SEA_LEVEL);
				}
			}
		}

		if(updateMapTexture)
		{
			mapTexture->update();
			publishMapInfo();
			EventAggregator::Instance().getEvent<UiEventCode>().publishEvent(UiEventCode(UIEVT_RUNNOISEUPDATE));
			updateMapTexture = false;
		}

		SDL_PumpEvents();
	}

	publishMapInfo();
}

void MapFrame::resetNoise()
{
	noiseArray[selectedNoise]->reset();
}

void MapFrame::updateMouseText()
{
	int mapX, mapY;
	
	if(mapPosFromMouse(&mapX, &mapY))
	{
		int h = mapArray[selectedMap]->getH(mapX, mapY);
		std::string text;

		if(mapTexture->getSeaRenderMode() == WITH_SEA && h <= mapArray[selectedMap]->getSeaLevel())
			text = "Sea";

		else
		{
			std::stringstream ss;
			ss << h;
			text = ss.str();
		}

		mouseTooltip->setText(text);

		int x, y;
		UiObject::getRelativeMousePos(this, &x, &y);
		mouseTooltip->setUiObjectOffset(x, y);
	}
}

bool MapFrame::mapPosFromMouse(int *x, int *y)
{
	if(!UiObject::getRelativeMousePos(this, x, y))
		return false;

	int mapFrameWidth = getWidth();
	int mapFrameHeight = getHeight();

	// mousePos / frameSize = mapPos / mapSize --> 
	// mapPos = (mousePos / frameSize) * mapSize
	*x = (*x / (double)mapFrameWidth) * mapArray[selectedMap]->getMapWidth() + 0.5; // +0.5 to round to the nearest int
	*y = (*y / (double)mapFrameHeight) * mapArray[selectedMap]->getMapHeight() + 0.5;

	return true;
}

bool MapFrame::customSdlEvtHandler(SDL_Event &e)
{
	switch(e.type)
	{
		case SDL_KEYDOWN:
			switch(e.key.keysym.sym)
			{
				case SDLK_UP:
					// up always shows sea (without increasing seaLevel on first press)
					if(mapTexture->getSeaRenderMode() != WITH_SEA)
						setSeaRenderMode(WITH_SEA);

					// up = +1 sea_Level when with_sea
					else if(!(e.key.keysym.mod & KMOD_SHIFT) && mapArray[selectedMap]->getSeaLevel() + 1 <= mapArray[selectedMap]->getHighestH())
						increaseSeaLevel();
				break;

				case SDLK_DOWN:
					// shift+down = no_sea (no update when not needed)
					if(e.key.keysym.mod & KMOD_SHIFT && mapTexture->getSeaRenderMode() != NO_SEA)
						setLandAndSeaRenderModes(FIXED, NO_SEA);

					// down = -1 seaLevel when with_sea
					else if(!(e.key.keysym.mod & KMOD_SHIFT) && mapArray[selectedMap]->getSeaLevel() - 1 >= 0 && mapTexture->getSeaRenderMode() == WITH_SEA)
						decreaseSeaLevel();
				break;

				case SDLK_z:
					if(mapTexture->getSeaRenderMode() != NO_SEA)
						setLandRenderMode(VARYING_HIGHEST);
				break;

				case SDLK_x:
					if(mapTexture->getSeaRenderMode() != NO_SEA)
						setLandRenderMode(VARYING_MAX);
				break;

				case SDLK_c:
					if(mapTexture->getSeaRenderMode() != NO_SEA)
						setLandRenderMode(FIXED);
				break;

				case SDLK_r:
					resetNoise();
					runNoise();
				break;

				case SDLK_n:
					normalizeMap(25);
				break;

				case SDLK_1:
					if(e.key.keysym.mod & KMOD_SHIFT)
						selectNoise(0);

					else if(!(e.key.keysym.mod & KMOD_SHIFT))
						selectMap(0);
				break;

				case SDLK_2:
					if(e.key.keysym.mod & KMOD_SHIFT)
						selectNoise(1);

					else if(!(e.key.keysym.mod & KMOD_SHIFT))
						selectMap(1);
				break;

				case SDLK_3:
					if(e.key.keysym.mod & KMOD_SHIFT)
						selectNoise(2);

					else if(!(e.key.keysym.mod & KMOD_SHIFT))
						selectMap(2);
				break;

				// if event was from keyboard but wasn't handled, returns to parent
				default:
					return false;
				break;
			}
		break;

		case SDL_MOUSEBUTTONUP:
			if(e.button.button == SDL_BUTTON_RIGHT)
			{
				int x, y;
				
				if(mapPosFromMouse(&x, &y))
					EventAggregator::Instance().getEvent<WalkWindowOpened>().publishEvent(WalkWindowOpened(mapArray[selectedMap], x, y));
			}
		break;

		case SDL_MOUSEMOTION:
			updateMouseText();
		break;

		// if event wasn't from keyboard, doesn't return to parent
		default:
			return true;
		break;
	}

	return true;
}

void MapFrame::selectMap(int i)
{
	selectedMap = i;

	mapTexture->setMapAndUpdate(mapArray[selectedMap]);

	publishMapInfo();
}

void MapFrame::selectNoise(int i)
{
	selectedNoise = i;

	publishMapInfo();
}

void MapFrame::normalizeMap(int n)
{
	mapArray[selectedMap]->normalize(n);
	
	// reset sea level after normalization
	mapArray[selectedMap]->setSeaLevel((mapArray[selectedMap]->getHighestH() / 2 ) - 1);
	
	mapTexture->update();

	publishMapInfo();
}

void MapFrame::increaseSeaLevel()
{
	mapArray[selectedMap]->increaseSeaLevel();

	mapTexture->update();

	publishMapInfo();
}

void MapFrame::decreaseSeaLevel()
{
	mapArray[selectedMap]->decreaseSeaLevel();

	mapTexture->update();

	publishMapInfo();
}

void MapFrame::setLandRenderMode(int mode)
{
	mapTexture->setLandRenderMode(mode);

	mapTexture->update();
}

void MapFrame::setSeaRenderMode(int mode)
{
	mapTexture->setSeaRenderMode(mode);

	mapTexture->update();
}

void MapFrame::setLandAndSeaRenderModes(int modeLand, int modeSea)
{
	mapTexture->setLandRenderMode(modeLand);
	mapTexture->setSeaRenderMode(modeSea);

	mapTexture->update();
}

void MapFrame::publishMapInfo()
{
	EventAggregator::Instance().getEvent<MapInfoUpdate>().publishEvent(MapInfoUpdate(	noiseArray[selectedNoise]->name, 
																						selectedMap + 1, 
																						mapArray[selectedMap]->getSeaLevel(), 
																						mapArray[selectedMap]->getHighestH(), 
																						mapArray[selectedMap]->getLowestH(), 
																						noiseArray[selectedNoise]->getPercentComplete()));
}
