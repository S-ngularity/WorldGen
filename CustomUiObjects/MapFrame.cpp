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
	// Map array
	mapArray = mapArr;
	numMaps = num;
	selectedMap = 0;

	// Noise array
	noiseArray[0] = new OpenSimplexNoise(mapArray[0], octaves, freq, persistence, freqDiv);
	noiseArray[1] = new DiamSqNoise(mapArray[0]);
	selectedNoise = 1;

	// Textures
	setParentUiManager(parentUiMngr);
	if(parentUiManager == NULL)
		std::cout << "MapFrame without parentUiManager (is NULL)." << std::endl;

	mapTexture = std::make_unique<MapTexture>(parentUiManager->getRenderer(), mapArray[selectedMap]);
	
	frameTexture = new std::unique_ptr<SdlTexture>[numMaps];
	for(int i = 0; i < numMaps; i++)
	{
		frameTexture[i] = std::make_unique<SdlTexture>(SDL_CreateTexture(parentUiManager->getRenderer(), 
																		SDL_PIXELFORMAT_RGBA8888, 
																		SDL_TEXTUREACCESS_TARGET, 
																		mapArray[i]->getMapWidth(), 
																		mapArray[i]->getMapHeight()));
	}
	//setUiObjectTexture(frameTexture);

	// Mouse tooltip for map height
	mouseTooltip = new UiLabel(0, 0, ALIGN_BOTTOM_LEFT, "", 20, 220, 20, 60);
	addChild(mouseTooltip);

	// Dragging functionality
	SDL_GetMouseState(&mouseLastX, &mouseLastY);
	mouseXDragOffset = 0;
	mouseYDragOffset = 0;
	clickHappenedHere = false;
	mapOffset = 0;
	setPreRenderProcedure([&](){ preRenderProcedure(); });

	// Zoom
	zoomX = 0;
	zoomH = 0;
	zoomW = frameTexture[selectedMap]->getWidth();
	zoomH = frameTexture[selectedMap]->getHeight();

	// Send map info update
	publishMapInfo();
}

MapFrame::~MapFrame()
{
	for(int i = 0; i < numNoises; i++)
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

	// framePos / frameSize = zoomOffsetPos / zoomSize
	// zoomOffsetPos = (framePos / frameSize) * zoomSize
	double xTemp = (*x / (double) getWidth()) * zoomW; // zoom offset pos from zoom top left pos
	double yTemp = (*y / (double) getHeight()) * zoomH;
	
	// zoomX+zoomOffsetPos / frameSize = mapPos / mapSize --> 
	// mapPos = (zoomX+zoomOffsetPos / frameSize) * mapSize
	*x = floor(((zoomX - mapOffset + xTemp) / (double) frameTexture[selectedMap]->getWidth()) * mapArray[selectedMap]->getMapWidth());
	*y = floor(((zoomY + yTemp) / (double) frameTexture[selectedMap]->getHeight()) * mapArray[selectedMap]->getMapHeight());

	if(*x < 0)
		*x = mapArray[selectedMap]->getMapWidth() + *x % mapArray[selectedMap]->getMapWidth();

	else if(*x >= mapArray[selectedMap]->getMapWidth())
		*x = *x % mapArray[selectedMap]->getMapWidth();

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
					noiseArray[selectedNoise]->reset();
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
					if(!(e.key.keysym.mod & KMOD_SHIFT))
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

			if(e.button.button == SDL_BUTTON_LEFT)
				clickHappenedHere = false;
		break;

		case SDL_MOUSEBUTTONDOWN:
			if(e.button.button == SDL_BUTTON_LEFT)
			{
				SDL_GetMouseState(&mouseLastX, &mouseLastY);
				mouseXDragOffset = 0;
				mouseYDragOffset = 0;
				clickHappenedHere = true;
			}
		break;

		#define ZOOM_AMOUNT 10

		case SDL_MOUSEWHEEL:
			if(e.wheel.y > 0)
			{
				if(zoomW != ZOOM_AMOUNT && zoomH != ZOOM_AMOUNT)
				{
					int x, y;

					if(!UiObject::getRelativeMousePos(this, &x, &y))
						break;

					zoomW -= ZOOM_AMOUNT;
					zoomH -= ZOOM_AMOUNT;
					
					if(zoomW < ZOOM_AMOUNT || zoomH < ZOOM_AMOUNT)
					{
						zoomW = ZOOM_AMOUNT;
						zoomH = ZOOM_AMOUNT;
					}

					zoomX += ZOOM_AMOUNT * x / (double) getWidth();
					if(zoomX > frameTexture[selectedMap]->getWidth() - zoomW)
						zoomX = frameTexture[selectedMap]->getWidth() - zoomW;

					zoomY += ZOOM_AMOUNT * y / (double) getHeight();
					if(zoomY > frameTexture[selectedMap]->getHeight() - zoomH)
						zoomY = frameTexture[selectedMap]->getHeight() - zoomH;
				}
			}

			else if(e.wheel.y < 0)
			{
				int x, y;

				if(!UiObject::getRelativeMousePos(this, &x, &y))
					return false;

				zoomW += ZOOM_AMOUNT;
				zoomH += ZOOM_AMOUNT;

				if(zoomW > frameTexture[selectedMap]->getWidth() || zoomH > frameTexture[selectedMap]->getHeight())
				{
					zoomW = frameTexture[selectedMap]->getWidth();
					zoomH = frameTexture[selectedMap]->getHeight();
				}

				zoomX -= ZOOM_AMOUNT * x / (double) getWidth();
				if(zoomX < 0)
					zoomX = 0;

				zoomY -= ZOOM_AMOUNT * y / (double) getHeight();
				if(zoomY < 0)
					zoomY = 0;

				if(zoomX + zoomW > frameTexture[selectedMap]->getWidth())
					zoomX = frameTexture[selectedMap]->getWidth() - zoomW;

				if(zoomY + zoomH > frameTexture[selectedMap]->getHeight())
					zoomY = frameTexture[selectedMap]->getHeight() - zoomH;
			}
		break;

		case SDL_MOUSEMOTION:
			if(clickHappenedHere && (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)))
			{
				int mouseX, mouseY;
				SDL_GetMouseState(&mouseX, &mouseY);

				// offset counter to avoid double<->int conversion problems
				mouseXDragOffset += (mouseX - mouseLastX) * zoomW / (double) getWidth();
				mouseYDragOffset += (mouseY - mouseLastY) * zoomH / (double) getHeight();

				// X drag is on mapOffset
				if(mouseXDragOffset > 1)
				{
					mapOffset += (int) mouseXDragOffset;
					mouseXDragOffset -= (int) mouseXDragOffset;
				}

				else if(mouseXDragOffset < -1)
				{
					mapOffset += (int) mouseXDragOffset;
					mouseXDragOffset -= (int) mouseXDragOffset;
				}

				// Y drag is on zoom point
				if(mouseYDragOffset > 1)
				{
					zoomY -= (int) mouseYDragOffset;
					if(zoomY < 0)
						zoomY = 0;
					mouseYDragOffset -= (int) mouseYDragOffset;
				}

				else if(mouseYDragOffset < -1)
				{
					zoomY -= (int) mouseYDragOffset;
					if(zoomY > frameTexture[selectedMap]->getHeight() - zoomH)
						zoomY = frameTexture[selectedMap]->getHeight() - zoomH;
					mouseYDragOffset -= (int) mouseYDragOffset;
				}

				mouseLastX = mouseX;
				mouseLastY = mouseY;
			}

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
	zoomX = 0;
	zoomY = 0;
	zoomW = frameTexture[selectedMap]->getWidth();
	zoomH = frameTexture[selectedMap]->getHeight();

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
	EventAggregator::Instance()
		.getEvent<MapInfoUpdate>()
			.publishEvent(MapInfoUpdate(noiseArray[selectedNoise]->name, 
										selectedMap + 1, 
										mapArray[selectedMap]->getSeaLevel(), 
										mapArray[selectedMap]->getHighestH(), 
										mapArray[selectedMap]->getLowestH(), 
										noiseArray[selectedNoise]->getPercentComplete()));
}


void MapFrame::preRenderProcedure()
{
	// Update clickHappenedHere if it was released outside this UiObject
	if(!(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)))
		clickHappenedHere = false;

	// Wrap mapOffset
	if(mapOffset > frameTexture[selectedMap]->getWidth())
		mapOffset = mapOffset % frameTexture[selectedMap]->getWidth();
	else if(mapOffset < 0)
		mapOffset = frameTexture[selectedMap]->getWidth() + mapOffset % frameTexture[selectedMap]->getWidth();

	// Print scrolling mapTexture to frameTexture
	frameTexture[selectedMap]->setAsRenderTarget(parentUiManager->getRenderer());

	mapTexture->renderFitToArea(parentUiManager->getRenderer(), 
									mapOffset - frameTexture[selectedMap]->getWidth(), 
									0, 
									frameTexture[selectedMap]->getWidth(), 
									frameTexture[selectedMap]->getHeight());	

	mapTexture->renderFitToArea(parentUiManager->getRenderer(), 
									mapOffset, 
									0, 
									frameTexture[selectedMap]->getWidth(), 
									frameTexture[selectedMap]->getHeight());

	frameTexture[selectedMap]->releaseRenderTarget(parentUiManager->getRenderer());

	frameTexture[selectedMap]->renderCutFitToArea(parentUiManager->getRenderer(),
										absoluteX, 
										absoluteY, 
										getWidth(), 
										getHeight(),
										zoomX, zoomY, zoomW, zoomH);
}
