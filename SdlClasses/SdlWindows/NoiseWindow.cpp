#include "SdlClasses/SdlWindows/NoiseWindow.h"

#include "Map.h"
#include "Noises/MyNoise.h"
#include "Noises/DiamSqNoise.h"
#include "Noises/OpenSimplexNoise.h"

#include "SdlClasses/UiObject.h"
#include "SdlClasses/UiCustomObjects/MapFrame.h"
#include "SdlClasses/UiEventCodes.h"

#include "SdlClasses/SdlTextures/MapTexture.h"

#include <iostream>
#include <iomanip>

#include <string>
#include <sstream>

using namespace std;

NoiseWindow::NoiseWindow(Map* mapVect[], int num) : 
	SdlWindow("WorldGen", 20, 40, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE), // superclass window constructor
	walkWindow(mapVect[0])
{
	evtAggregator.addUiEventObserver(this);

	setWindowSdlEvtHandler([&](SDL_Event &e){handleSdlEvent(e);});

	this->mapVect = mapVect;
	numMaps = num;

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	//SDL_RenderSetLogicalSize(getRenderer(), mapVect[selectedMap]->getMapWidth(), mapVect[selectedMap]->getMapHeight());

	createGui();

	// initialize renderer color
	SDL_SetRenderDrawColor(getRenderer(), 0, 0, 0, 255);

	gui->renderScaled(getRenderer(), 0, 0, getWindowWidthScale(), getWindowHeightScale());
	refresh();
}

NoiseWindow::~NoiseWindow()
{
	evtAggregator.removeUiEventObserver(this);
}

bool NoiseWindow::handleUiEvent(int evtId)
{
	if(evtId == UIEVT_CONTENTSCHANGED)
	{
		gui->renderScaled(getRenderer(), 0, 0, getWindowWidthScale(), getWindowHeightScale());
		refresh();
	}

	return true;
}

void NoiseWindow::handleSdlEvent(SDL_Event& e)
{
	walkWindow.handleEvent(e);

	if(hasKeyboardFocus())
	{
		if(gui != NULL)
			gui->handleEvent(e);
	}
}

// GUI

SDL_Texture* NoiseWindow::createDrawnTexture(int width, int height, int r, int g, int b, int a)
{
	SDL_Texture *targetTex = SDL_CreateTexture(getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
	SDL_SetRenderTarget(getRenderer(), targetTex);
	SDL_SetRenderDrawColor(getRenderer(), r, g, b, a);
	SDL_RenderClear(getRenderer());
	SDL_SetRenderTarget(getRenderer(), NULL);

	return targetTex;
}

void NoiseWindow::createGui()
{
	gui = new UiObject(	0, 0, getWindowWidth(), getWindowHeight(), NULL);


	auto mapTex = new MapTexture(getRenderer(), mapVect[0]);
	// mapFrame has evtAggregator so it can publish events (to be treated here in NoiseWindow)
	// and observe events from other UiObjects
	gui->addChild(new MapFrame(	0, 0, getWindowWidth() - SIDEBAR_WIDTH, getWindowHeight(), 
								mapTex, mapVect, numMaps, &evtAggregator));

	SdlTexture *bgTex = new SdlTexture(createDrawnTexture(SIDEBAR_WIDTH, gui->getHeight(), 0, 126, 126, 255), SIDEBAR_WIDTH, gui->getHeight());
	auto bgUi = new UiObject(gui->getWidth() - SIDEBAR_WIDTH, 0,  bgTex, 
							 NULL);

	SdlTexture *btMap1Tex = new SdlTexture(createDrawnTexture(30, 30, 0, 255, 0, 255), 30, 30);
	bgUi->addChild(new UiObject(30, 30, btMap1Tex, 
								[&](SDL_Event &e){return btMapClicked(e, 0);}));

	SdlTexture *btMap2Tex = new SdlTexture(createDrawnTexture(30, 30, 0, 255, 0, 255), 30, 30);
	bgUi->addChild(new UiObject(80, 30, btMap2Tex, 
								[&](SDL_Event &e){return btMapClicked(e, 1);}));

	SdlTexture *btMap3Tex = new SdlTexture(createDrawnTexture(30, 30, 0, 255, 0, 255), 30, 30);
	bgUi->addChild(new UiObject(130, 30, btMap3Tex, 
								[&](SDL_Event &e){return btMapClicked(e, 2);}));

	SdlTexture *btNoise1Tex = new SdlTexture(createDrawnTexture(30, 30, 0, 255, 0, 255), 30, 30);
	bgUi->addChild(new UiObject(30, 80, btNoise1Tex, 
								[&](SDL_Event &e){return btNoiseClicked(e, 0);}));

	SdlTexture *btNoise2Tex = new SdlTexture(createDrawnTexture(30, 30, 0, 255, 0, 255), 30, 30);
	bgUi->addChild(new UiObject( 80, 80, btNoise2Tex, 
								[&](SDL_Event &e){return btNoiseClicked(e, 1);}));

	SdlTexture *btNoise3Tex = new SdlTexture(createDrawnTexture(30, 30, 0, 255, 0, 255), 30, 30);
	bgUi->addChild(new UiObject(	130, 80, btNoise3Tex, 
								[&](SDL_Event &e){return btNoiseClicked(e, 2);}));

	gui->addChild(bgUi);
}

bool NoiseWindow::btMapClicked(SDL_Event &e, int i)
{
	if(e.type == SDL_MOUSEBUTTONUP)
	{
		switch(i)
		{
			case 0:
				evtAggregator.publishUiEvent(UIEVT_BTCLICKEDMAP0);
			break;

			case 1:
				evtAggregator.publishUiEvent(UIEVT_BTCLICKEDMAP1);
			break;

			case 2:
				evtAggregator.publishUiEvent(UIEVT_BTCLICKEDMAP2);
			break;
		}

		return true;
	}

	else
		return false;
}

bool NoiseWindow::btNoiseClicked(SDL_Event &e, int i)
{
	if(e.type == SDL_MOUSEBUTTONUP)
	{
		switch(i)
		{
			case 0:
				evtAggregator.publishUiEvent(UIEVT_BTCLICKEDNOISE0);
			break;

			case 1:
				evtAggregator.publishUiEvent(UIEVT_BTCLICKEDNOISE1);
			break;

			case 2:
				evtAggregator.publishUiEvent(UIEVT_BTCLICKEDNOISE2);
			break;
		}

		return true;
	}

	else
		return false;
}
