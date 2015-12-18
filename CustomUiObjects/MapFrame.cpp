#include "CustomUiObjects/MapFrame.h"

#include "Map/Map.h"
//#include "Map/Noises/MyNoise.h"
#include "Map/Noises/DiamSqNoise.h"
#include "Map/Noises/OpenSimplexNoise.h"

#include "CustomSdlTextures/MapTexture.h"

#include "Ui/UiManager.h"
#include "DefaultUiObjects/UiLabel.h"

#include "Ui/EvtAggr.h"
#include "Ui/UiEvents/UiCode.h"
#include "Ui/UiEvents/WalkWindowOpened.h"
#include "Ui/UiEvents/MapInfoUpdate.h"

#include <iostream>
#include <iomanip>

#include <string>
#include <sstream>

MapFrame::MapFrame(int x, int y, int w, int h, std::vector<std::shared_ptr<Map>> &mapVec) : 
	UiObject(x, y, w, h, NULL,
			[&](SDL_Event &e){return customSdlEvtHandler(e);}), 
	selectedMapIdx(0), 
	selectedNoiseIdx(1), 
	mapVector(mapVec)
{
	// Noise vector
	noiseVector.push_back(std::make_unique<OpenSimplexNoise>(mapVector.at(selectedMapIdx), octaves, freq, persistence, freqDiv));
	noiseVector.push_back(std::make_unique<DiamSqNoise>(mapVector.at(selectedMapIdx)));
}

void MapFrame::init()
{
	// Textures
	if(parentUiManager == NULL)
		std::cout << "MapFrame without parentUiManager (is NULL)." << std::endl;

	mapTexture = std::make_unique<MapTexture>(parentUiManager->getRenderer(), mapVector.at(selectedMapIdx));
	
	frameTexture = std::make_shared<SdlTexture>(SDL_CreateTexture(parentUiManager->getRenderer(), 
																		SDL_PIXELFORMAT_RGBA8888, 
																		SDL_TEXTUREACCESS_TARGET, 
																		mapTexture->getWidth(), 
																		mapTexture->getHeight()));
	setUiObjectTexture(frameTexture);

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
	zoomY = 0;
	zoomW = frameTexture->getWidth();
	zoomH = frameTexture->getHeight();

	// Send map info update
	publishMapInfo();
}

void MapFrame::runNoise()
{
	bool updateMapTexture = false;
	int shownPercent = 0, updateAt = 0;

	noiseVector[selectedNoiseIdx]->setMap(mapVector.at(selectedMapIdx));

	mapTexture->setSeaRenderMode(NO_SEA);  // no sea while not done
	mapTexture->setLandRenderMode(FIXED);

	while(noiseVector[selectedNoiseIdx]->getPercentComplete() < 100) // noise iterations
	{
		noiseVector[selectedNoiseIdx]->runOnce();

		// update only once per percent
		if(noiseVector[selectedNoiseIdx]->getPercentComplete() != shownPercent)
		{
			shownPercent = noiseVector[selectedNoiseIdx]->getPercentComplete();

			if(shownPercent >= updateAt || shownPercent >= 100)
			{
				updateAt += UPDATE_AT_PERCENT;
				updateMapTexture = true;

				if(shownPercent >= 100)
				{
					mapTexture->setSeaRenderMode(WITH_SEA);
					mapTexture->setLandRenderMode(VARYING_HIGHEST);

					mapVector.at(selectedMapIdx)->setSeaLevel(SEA_LEVEL);
				}
			}
		}

		if(updateMapTexture)
		{
			mapTexture->update();
			publishMapInfo();
			EvtAggr::publish<UiCode>(UiCode(UIEVT_RUNNOISEUPDATE));
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
		int h = mapVector.at(selectedMapIdx)->getH(mapX, mapY);
		std::string text;

		if(mapTexture->getSeaRenderMode() == WITH_SEA && h <= mapVector.at(selectedMapIdx)->getSeaLevel())
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
	double xTemp = (*x / (double) getWidth()) * zoomW; // zoom offset pos (on mouse pointer) from zoom top left pos
	double yTemp = (*y / (double) getHeight()) * zoomH;
	
	*x = floor(zoomX + xTemp - mapOffset);
	*y = floor(zoomY + yTemp);

	// wrap mapOffset
	if(*x < 0)
		*x = mapVector.at(selectedMapIdx)->getMapWidth() + *x % mapVector.at(selectedMapIdx)->getMapWidth();

	else if(*x >= mapVector.at(selectedMapIdx)->getMapWidth())
		*x = *x % mapVector.at(selectedMapIdx)->getMapWidth();

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
					else if(!(e.key.keysym.mod & KMOD_SHIFT) && mapVector.at(selectedMapIdx)->getSeaLevel() + 1 <= mapVector.at(selectedMapIdx)->getHighestH())
						increaseSeaLevel();
				break;

				case SDLK_DOWN:
					// shift+down = no_sea (no update when not needed)
					if(e.key.keysym.mod & KMOD_SHIFT && mapTexture->getSeaRenderMode() != NO_SEA)
						setLandAndSeaRenderModes(FIXED, NO_SEA);

					// down = -1 seaLevel when with_sea
					else if(!(e.key.keysym.mod & KMOD_SHIFT) && mapVector.at(selectedMapIdx)->getSeaLevel() - 1 >= 0 && mapTexture->getSeaRenderMode() == WITH_SEA)
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
					noiseVector[selectedNoiseIdx]->reset();
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
			// on right click release, open WalkWindow
			if(e.button.button == SDL_BUTTON_RIGHT)
			{
				int x, y;
				
				if(mapPosFromMouse(&x, &y))
					EvtAggr::publish<WalkWindowOpened>(WalkWindowOpened(mapVector.at(selectedMapIdx), x, y));
			}

			if(e.button.button == SDL_BUTTON_LEFT)
				clickHappenedHere = false;
		break;

		case SDL_MOUSEBUTTONDOWN:
			// on left click press, setup variables for dragging
			if(e.button.button == SDL_BUTTON_LEFT)
			{
				SDL_GetMouseState(&mouseLastX, &mouseLastY);
				mouseXDragOffset = 0;
				mouseYDragOffset = 0;
				clickHappenedHere = true;
			}
		break;

		case SDL_MOUSEMOTION:
			// if click began inside and it's pressed
			if(clickHappenedHere && (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)))
			{
				int mouseX, mouseY;
				SDL_GetMouseState(&mouseX, &mouseY);

				// offset counter to avoid double<->int conversion problems
				// mouse motion * zoom scale * inverse of window scale
				mouseXDragOffset += (mouseX - mouseLastX) * (zoomW / (double) getWidth()) * (1 / parentUiManager->getWindowScaleW());
				mouseYDragOffset += (mouseY - mouseLastY) * (zoomH / (double) getHeight()) * (1 / parentUiManager->getWindowScaleH());

				// when drag offset is over 1, drag all but the fractional part
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
					if(zoomY > frameTexture->getHeight() - zoomH)
						zoomY = frameTexture->getHeight() - zoomH;
					mouseYDragOffset -= (int) mouseYDragOffset;
				}

				// last mouse pos is where it is now
				mouseLastX = mouseX;
				mouseLastY = mouseY;
			}

			updateMouseText();
		break;

		#define ZOOM_AMOUNT (int)(frameTexture->getWidth() * 0.05)

		case SDL_MOUSEWHEEL:
			// wheel up
			if(e.wheel.y > 0)
			{
				// if not on max zoom
				if(zoomW != ZOOM_AMOUNT && zoomH != ZOOM_AMOUNT)
				{
					int x, y;

					// relative mouse pos from this UiObject 0,0, scaled 
					// to the original size (even if the window is resized)
					if(!UiObject::getRelativeMousePos(this, &x, &y))
						break;

					// reduce zoomed area
					zoomW -= ZOOM_AMOUNT;
					zoomH -= ZOOM_AMOUNT;
					
					// limit zoomed area (max zoom)
					if(zoomW < ZOOM_AMOUNT || zoomH < ZOOM_AMOUNT)
					{
						zoomW = ZOOM_AMOUNT;
						zoomH = ZOOM_AMOUNT;
					}

					// move zoomed area relative to the mouse pointer
					zoomX += ZOOM_AMOUNT * x / (double) getWidth() * 1.01;	// 1.01 to compensate mouse round error on the UiObjects limit
					if(zoomX > frameTexture->getWidth() - zoomW)			// (more accurately center the zoom on the mouse)
						zoomX = frameTexture->getWidth() - zoomW;

					zoomY += ZOOM_AMOUNT * y / (double) getHeight() * 1.01;
					if(zoomY > frameTexture->getHeight() - zoomH)
						zoomY = frameTexture->getHeight() - zoomH;
				}
			}

			else if(e.wheel.y < 0)
			{
				int x, y;

				if(!UiObject::getRelativeMousePos(this, &x, &y))
					return false;

				// enlarge zoomed area
				zoomW += ZOOM_AMOUNT;
				zoomH += ZOOM_AMOUNT;

				// limit zoomed area
				if(zoomW > frameTexture->getWidth() || zoomH > frameTexture->getHeight())
				{
					zoomW = frameTexture->getWidth();
					zoomH = frameTexture->getHeight();
				}

				// move zoomed area relative to the mouse pointer
				zoomX -= ZOOM_AMOUNT * x / (double) getWidth();
				if(zoomX < 0)
					zoomX = 0;

				zoomY -= ZOOM_AMOUNT * y / (double) getHeight();
				if(zoomY < 0)
					zoomY = 0;

				// avoid zoomed area to go beyond texture limits
				if(zoomX + zoomW > frameTexture->getWidth())
					zoomX = frameTexture->getWidth() - zoomW;

				if(zoomY + zoomH > frameTexture->getHeight())
					zoomY = frameTexture->getHeight() - zoomH;
			}
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
	selectedMapIdx = i;

	mapTexture->setMapAndUpdate(mapVector.at(selectedMapIdx));

	if(frameTexture->getWidth() != mapTexture->getWidth()
		 || frameTexture->getHeight() != mapTexture->getHeight())
	{
		frameTexture = std::make_shared<SdlTexture>(SDL_CreateTexture(parentUiManager->getRenderer(), 
																		SDL_PIXELFORMAT_RGBA8888, 
																		SDL_TEXTUREACCESS_TARGET, 
																		mapVector.at(selectedMapIdx)->getMapWidth(), 
																		mapVector.at(selectedMapIdx)->getMapHeight()));
		
		setUiObjectTexture(frameTexture);
	}

	zoomX = 0;
	zoomY = 0;
	zoomW = frameTexture->getWidth();
	zoomH = frameTexture->getHeight();

	publishMapInfo();
}

void MapFrame::selectNoise(int i)
{
	selectedNoiseIdx = i;

	publishMapInfo();
}

void MapFrame::normalizeMap(int n)
{
	mapVector.at(selectedMapIdx)->normalize(n);
	
	// reset sea level after normalization
	mapVector.at(selectedMapIdx)->setSeaLevel((mapVector.at(selectedMapIdx)->getHighestH() / 2 ) - 1);
	
	mapTexture->update();

	publishMapInfo();
}

void MapFrame::increaseSeaLevel()
{
	mapVector.at(selectedMapIdx)->increaseSeaLevel();

	mapTexture->update();

	publishMapInfo();
}

void MapFrame::decreaseSeaLevel()
{
	mapVector.at(selectedMapIdx)->decreaseSeaLevel();

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
	EvtAggr::publish<MapInfoUpdate>(MapInfoUpdate(noiseVector[selectedNoiseIdx]->name, 
												selectedMapIdx + 1, 
												mapVector.at(selectedMapIdx)->getSeaLevel(), 
												mapVector.at(selectedMapIdx)->getHighestH(), 
												mapVector.at(selectedMapIdx)->getLowestH(), 
												noiseVector[selectedNoiseIdx]->getPercentComplete()));
}


void MapFrame::preRenderProcedure()
{
	// Update clickHappenedHere if it was released outside this UiObject
	if(!(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)))
		clickHappenedHere = false;

	// Wrap mapOffset
	if(mapOffset > frameTexture->getWidth())
		mapOffset = mapOffset % frameTexture->getWidth();
	else if(mapOffset < 0)
		mapOffset = frameTexture->getWidth() + mapOffset % frameTexture->getWidth();

	// Print scrolling mapTexture to frameTexture
	frameTexture->setAsRenderTarget(parentUiManager->getRenderer());

	mapTexture->renderFitToArea(parentUiManager->getRenderer(), 
									mapOffset - frameTexture->getWidth(), 
									0, 
									frameTexture->getWidth(), 
									frameTexture->getHeight());	

	mapTexture->renderFitToArea(parentUiManager->getRenderer(), 
									mapOffset, 
									0, 
									frameTexture->getWidth(), 
									frameTexture->getHeight());

	frameTexture->releaseRenderTarget(parentUiManager->getRenderer());

	frameTexture->setCropRect(zoomX, zoomY, zoomW, zoomH);
}
