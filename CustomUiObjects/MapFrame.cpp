#include "CustomUiObjects/MapFrame.h"

#include "Map/Map.h"

#include "CustomSdlTextures/MapTexture.h"

#include "Ui/UiManager.h"
#include "DefaultUiObjects/UiLabel.h"

#include "Ui/EvtAggr.h"
#include "Ui/UiEvents/UiCode.h"
#include "Ui/UiEvents/WalkWindowOpened.h"

#include <iostream>
#include <iomanip>

#include <string>
#include <sstream>

MapFrame::MapFrame(int x, int y, int w, int h, std::shared_ptr<Map> &selectedMapPtr) : 
	UiObject(x, y, w, h, NULL,
			[&](SDL_Event &e){return customSdlEvtHandler(e);}), 
	selectedMap(selectedMapPtr)
{}

void MapFrame::init()
{
	// Textures
	if(parentUiManager == NULL)
		std::cout << "MapFrame without parentUiManager (is NULL)." << std::endl;

	mapTexture = std::make_unique<MapTexture>(parentUiManager->getRenderer(), *selectedMap);
	
	setUiObjectTexture(std::make_shared<SdlTexture>(SDL_CreateTexture(parentUiManager->getRenderer(), 
																		SDL_PIXELFORMAT_RGBA8888, 
																		SDL_TEXTUREACCESS_TARGET, 
																		mapTexture->getWidth(), 
																		mapTexture->getHeight())));

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
	zoomW = getTexture()->getWidth();
	zoomH = getTexture()->getHeight();
}

void MapFrame::updateMouseText()
{
	int mapX, mapY;
	
	if(mapPosFromMouse(&mapX, &mapY))
	{
		int h = selectedMap->getH(mapX, mapY);
		std::string text;

		if(mapTexture->getSeaRenderMode() == WITH_SEA && h <= selectedMap->getSeaLevel())
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

	// zoom offset pos (on mouse pointer) from zoom top left pos
	// regra de três da posição relativa do mouse dentro do tamanho do frame para 
	// a posição relativa do mouse dentro do tamanho da área de zoom:
	// framePos / frameSize = zoomOffsetPos / zoomSize
	// zoomOffsetPos = (framePos / frameSize) * zoomSize
	double xTemp = (*x / (double) getWidth()) * zoomW;
	double yTemp = (*y / (double) getHeight()) * zoomH;
	
	// zoom area top left pos (on the map - because the mapTexture has the same dimensions as the original map)
	// + mouse relative pos inside zoomed area
	*x = floor(zoomX + xTemp - mapOffset);
	*y = floor(zoomY + yTemp);

	// wrap mapOffset
	if(*x < 0)
		*x = selectedMap->getMapWidth() + *x % selectedMap->getMapWidth();

	else if(*x >= selectedMap->getMapWidth())
		*x = *x % selectedMap->getMapWidth();

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
					{
						setSeaRenderMode(WITH_SEA);
						return true;
					}

					return false;
				break;

				case SDLK_DOWN:
					// shift+down = no_sea (no update when not needed)
					if(e.key.keysym.mod & KMOD_SHIFT && mapTexture->getSeaRenderMode() != NO_SEA)
						setLandAndSeaRenderModes(FIXED, NO_SEA);
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
					EvtAggr::publish<WalkWindowOpened>(WalkWindowOpened(*selectedMap, x, y));
			}

			// left click release
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
					if(zoomY > getTexture()->getHeight() - zoomH)
						zoomY = getTexture()->getHeight() - zoomH;
					mouseYDragOffset -= (int) mouseYDragOffset;
				}

				// last mouse pos is where it is now
				mouseLastX = mouseX;
				mouseLastY = mouseY;
			}

			updateMouseText();
		break;

		case SDL_MOUSEWHEEL:
			#define ZOOM_AMOUNT (int)(getTexture()->getWidth() * 0.05)
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

					else
					{
						// move zoomed area relative to the mouse pointer
						zoomX += ZOOM_AMOUNT * x / (double) getWidth() * 1.01; // 1.01 to compensate mouse round error on the UiObjects limit
						zoomY += ZOOM_AMOUNT * y / (double) getHeight() * 1.01;
					}

					if(zoomX > getTexture()->getWidth() - zoomW)
						zoomX = getTexture()->getWidth() - zoomW;

					if(zoomY > getTexture()->getHeight() - zoomH)
						zoomY = getTexture()->getHeight() - zoomH;
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
				if(zoomW > getTexture()->getWidth() || zoomH > getTexture()->getHeight())
				{
					zoomW = getTexture()->getWidth();
					zoomH = getTexture()->getHeight();
				}

				// move zoomed area relative to the mouse pointer
				zoomX -= ZOOM_AMOUNT * x / (double) getWidth();
				if(zoomX < 0)
					zoomX = 0;

				zoomY -= ZOOM_AMOUNT * y / (double) getHeight();
				if(zoomY < 0)
					zoomY = 0;

				// avoid zoomed area to go beyond texture limits
				if(zoomX + zoomW > getTexture()->getWidth())
					zoomX = getTexture()->getWidth() - zoomW;

				if(zoomY + zoomH > getTexture()->getHeight())
					zoomY = getTexture()->getHeight() - zoomH;
			}
		break;

		// if event wasn't from keyboard, doesn't return to parent
		default:
			return true;
		break;
	}

	return true;
}

void MapFrame::updateTexture()
{
	mapTexture->update(*selectedMap);

	if(getTexture()->getWidth() != mapTexture->getWidth()
		 || getTexture()->getHeight() != mapTexture->getHeight())
	{
		setUiObjectTexture(std::make_shared<SdlTexture>(SDL_CreateTexture(parentUiManager->getRenderer(), 
																		SDL_PIXELFORMAT_RGBA8888, 
																		SDL_TEXTUREACCESS_TARGET, 
																		selectedMap->getMapWidth(), 
																		selectedMap->getMapHeight())));

		resetZoom();
	}
}

void MapFrame::resetZoom()
{
	zoomX = 0;
	zoomY = 0;
	zoomW = getTexture()->getWidth();
	zoomH = getTexture()->getHeight();
}

void MapFrame::setLandRenderMode(int mode)
{
	mapTexture->setLandRenderMode(mode);
	mapTexture->update(*selectedMap);
}

void MapFrame::setSeaRenderMode(int mode)
{
	mapTexture->setSeaRenderMode(mode);
	mapTexture->update(*selectedMap);
}

void MapFrame::setLandAndSeaRenderModes(int modeLand, int modeSea)
{
	mapTexture->setLandRenderMode(modeLand);
	mapTexture->setSeaRenderMode(modeSea);
	mapTexture->update(*selectedMap);
}

void MapFrame::preRenderProcedure()
{
	// Update clickHappenedHere if it was released outside this UiObject
	if(!(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)))
		clickHappenedHere = false;

	// Wrap mapOffset
	if(mapOffset > getTexture()->getWidth())
		mapOffset = mapOffset % getTexture()->getWidth();
	else if(mapOffset < 0)
		mapOffset = getTexture()->getWidth() + mapOffset % getTexture()->getWidth();

	// Print scrolling mapTexture to MapFrame's UiObject texture
	getTexture()->setAsRenderTarget(parentUiManager->getRenderer());

	mapTexture->renderFitToArea(parentUiManager->getRenderer(), 
									mapOffset - getTexture()->getWidth(), 
									0, 
									getTexture()->getWidth(), 
									getTexture()->getHeight());	

	mapTexture->renderFitToArea(parentUiManager->getRenderer(), 
									mapOffset, 
									0, 
									getTexture()->getWidth(), 
									getTexture()->getHeight());

	getTexture()->releaseRenderTarget(parentUiManager->getRenderer());

	// Set zoomed area on the texture
	getTexture()->setCropRect(zoomX, zoomY, zoomW, zoomH);
}
