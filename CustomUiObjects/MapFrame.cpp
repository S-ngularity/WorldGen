#include "CustomUiObjects/MapFrame.h"

#include "Map/Map.h"
//#include "Map/Noises/MyNoise.h"
#include "Map/Noises/DiamSqNoise.h"
#include "Map/Noises/OpenSimplexNoise.h"

#include "CustomSdlTextures/MapTexture.h"

#include "Ui/EventAggregator.h"
#include "Ui/UiEvents/UiEventCode.h"
#include "Ui/UiEvents/WalkWindowOpened.h"

#include <iostream>
#include <iomanip>

#include <string>
#include <sstream>

MapFrame::MapFrame(SDL_Renderer *r, int x, int y, int w, int h, Map* mapArr[], int num) : 
	UiObject(r, x, y, w, h, NULL,
			[&](SDL_Event &e){return customSdlEvtHandler(e);})
{
	mapArray = mapArr;
	numMaps = num;
	selectedMap = 0;

	noiseArray[0] = new OpenSimplexNoise(mapArray[0], octaves, freq, persistence, freqDiv);
	noiseArray[1] = new DiamSqNoise(mapArray[0]);
	selectedNoise = 0;

	mapTexture = new MapTexture(getRenderer(), mapArray[selectedMap]);
	mapTexture->update();
	setUiObjectTexture(mapTexture);

	mouseText = new MouseHeightText(getRenderer());
	setPostRenderProcedure([&]() { mouseText->render(getAbsoluteX(), getAbsoluteY()); });
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

	std::cout << std::endl << std::endl << std::setw(2) << std::setfill('0') << shownPercent << "%" << std::flush;

	while(noiseArray[selectedNoise]->getPercentComplete() < 100) // noise iterations
	{
		noiseArray[selectedNoise]->runOnce();

		// update only once per percent
		if(noiseArray[selectedNoise]->getPercentComplete() != shownPercent)
		{
			shownPercent = noiseArray[selectedNoise]->getPercentComplete();

			std::cout << "\b\b\b" << std::setw(2) << std::setfill('0') << shownPercent << "%" << std::flush;

			if(shownPercent >= updateAt || shownPercent == 100)
			{
				updateAt += UPDATE_AT_PERCENT;

				if(noiseArray[selectedNoise]->getPercentComplete() < 100) // no sea while not done
				{
					mapTexture->setSeaRenderMode(NO_SEA);
					mapTexture->setLandRenderMode(FIXED);
					updateMapTexture = true;
				}

				else
				{
					mapTexture->setSeaRenderMode(WITH_SEA);
					mapTexture->setLandRenderMode(VARYING_HIGHEST);

					std::cout << std::endl 
					<< std::endl << "Highest point: " << mapArray[selectedMap]->getHighestH()
					<< std::endl << "Lowest point: " << mapArray[selectedMap]->getLowestH() << std::endl << std::endl;

					std::cout << "Sea Level : " << std::setw(3) << std::setfill('0') << mapArray[selectedMap]->getSeaLevel() << std::flush;
					
					updateMapTexture = true; // last noise print
				}
			}
		}

		if(updateMapTexture)
		{
			mapTexture->update();
			EventAggregator::Instance().getEvent<UiEventCode>().publishEvent(UiEventCode(UIEVT_RUNNOISEUPDATE));
			updateMapTexture = false;
		}

		SDL_PumpEvents();
	}

	mapArray[selectedMap]->setSeaLevel(SEA_LEVEL);
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

		mouseText->update(text);

		int x, y;
		UiObject::getRelativeMousePos(this, &x, &y);
		mouseText->setUiObjectOffset(x, y - 30);
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
	*x = (*x / (double)mapFrameWidth) * mapArray[selectedMap]->getMapWidth();
	*y = (*y / (double)mapFrameHeight) * mapArray[selectedMap]->getMapHeight();

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
					normalizeMap();
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
	std::cout << std::endl << std::endl << "Map " << selectedMap+1 << std::endl;
	std::cout << "Sea Level : " << std::setw(3) << std::setfill('0') << mapArray[selectedMap]->getSeaLevel() << std::flush;
}

void MapFrame::selectNoise(int i)
{
	selectedNoise = i;
}

void MapFrame::normalizeMap()
{
	int n;

	std::cout << std::endl << "Normalize: " << std::flush;
	std::cin >> n;

	mapArray[selectedMap]->normalize(n);
	
	// reset sea level after normalization
	mapArray[selectedMap]->setSeaLevel((mapArray[selectedMap]->getHighestH() / 2 ) - 1);
	
	std::cout << "Sea Level : " << std::setw(3) << std::setfill('0') << mapArray[selectedMap]->getSeaLevel() << std::flush;
	
	mapTexture->update();
}

void MapFrame::increaseSeaLevel()
{
	mapArray[selectedMap]->increaseSeaLevel();
	std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << "Sea Level : " << std::setw(3) << std::setfill('0') << mapArray[selectedMap]->getSeaLevel() << std::flush;

	mapTexture->update();
}

void MapFrame::decreaseSeaLevel()
{
	mapArray[selectedMap]->decreaseSeaLevel();
	std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << "Sea Level : " << std::setw(3) << std::setfill('0') << mapArray[selectedMap]->getSeaLevel() << std::flush;

	mapTexture->update();
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