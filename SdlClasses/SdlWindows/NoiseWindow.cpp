#include "SdlClasses/SdlWindows/NoiseWindow.h"

#include "Map.h"

#include "SdlClasses/UiObject.h"
#include "SdlClasses/UiCustomObjects/MapFrame.h"
#include "SdlClasses/UiEventCodes.h"

#include "SdlClasses/SdlTextures/MapTexture.h"


NoiseWindow::NoiseWindow(Map* mapArr[], int num) : 
	SdlWindow(	"WorldGen", 20, 40, 
				SCREEN_WIDTH, SCREEN_HEIGHT, // window size 
				SCREEN_WIDTH, SCREEN_HEIGHT, // window resolution
				SDL_WINDOW_RESIZABLE, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE), // superclass window constructor
	walkWindow(mapArr[0])
{
	addChildWindow(&walkWindow);

	addUiEventObserver(this);
	setWindowSdlEvtHandler([&](SDL_Event &e){return handleInternalSdlEvent(e);});

	mapArray = mapArr;
	numMaps = num;

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	//SDL_RenderSetLogicalSize(getRenderer(), mapArray[selectedMap]->getMapWidth(), mapArray[selectedMap]->getMapHeight());

	createGui();

	signalRefresh();
}

NoiseWindow::~NoiseWindow()
{
	removeUiEventObserver(this);
}

bool NoiseWindow::handleUiEvent(int evtId)
{
	if(evtId == UIEVT_CONTENTSCHANGED)
		signalRefresh();

	return true;
}

bool NoiseWindow::handleInternalSdlEvent(SDL_Event& e)
{
	//return walkWindow.handleEvent(e);
	return true;
}

// UI

SDL_Texture* NoiseWindow::createDrawnTexture(int width, int height, int r, int g, int b, int a)
{
	SDL_Texture *targetTex = SDL_CreateTexture(getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
	SDL_SetRenderTarget(getRenderer(), targetTex);
	SDL_SetRenderDrawColor(getRenderer(), r, g, b, a);
	SDL_RenderClear(getRenderer());
	SDL_SetRenderTarget(getRenderer(), NULL);

	return targetTex;
}

MapFrame *mapFrame = nullptr;

void NoiseWindow::createGui()
{
	mapFrame = new MapFrame(getRenderer(), 0, 0, gui->getWidth() - SIDEBAR_WIDTH, gui->getHeight(), mapArray, numMaps);
	gui->addChild(mapFrame);

	SdlTexture *bgTex = new SdlTexture(createDrawnTexture(SIDEBAR_WIDTH, gui->getHeight(), 0, 126, 126, 255), SIDEBAR_WIDTH, gui->getHeight());
	auto bgUi = new UiObject(getRenderer(), gui->getWidth() - SIDEBAR_WIDTH, 0,  bgTex, 
							 nullptr);

	SdlTexture *btMap1Tex = new SdlTexture(createDrawnTexture(30, 30, 0, 255, 0, 255), 30, 30);
	bgUi->addChild(new UiObject(getRenderer(), 30, 30, btMap1Tex, 
								[&](SDL_Event &e){ return btMapClicked(e, 0); }));

	SdlTexture *btMap2Tex = new SdlTexture(createDrawnTexture(30, 30, 0, 255, 0, 255), 30, 30);
	bgUi->addChild(new UiObject(getRenderer(), 80, 30, btMap2Tex, 
								[&](SDL_Event &e){ return btMapClicked(e, 1); }));

	SdlTexture *btMap3Tex = new SdlTexture(createDrawnTexture(30, 30, 0, 255, 0, 255), 30, 30);
	bgUi->addChild(new UiObject(getRenderer(), 130, 30, btMap3Tex, 
								[&](SDL_Event &e){ return btMapClicked(e, 2); }));

	SdlTexture *btNoise1Tex = new SdlTexture(createDrawnTexture(30, 30, 0, 255, 0, 255), 30, 30);
	bgUi->addChild(new UiObject(getRenderer(), 30, 80, btNoise1Tex, 
								[&](SDL_Event &e){ return btNoiseClicked(e, 0); }));

	SdlTexture *btNoise2Tex = new SdlTexture(createDrawnTexture(30, 30, 0, 255, 0, 255), 30, 30);
	bgUi->addChild(new UiObject(getRenderer(), 80, 80, btNoise2Tex, 
								[&](SDL_Event &e){ return btNoiseClicked(e, 1); }));

	SdlTexture *btNoise3Tex = new SdlTexture(createDrawnTexture(30, 30, 0, 255, 0, 255), 30, 30);
	bgUi->addChild(new UiObject(getRenderer(), 130, 80, btNoise3Tex, 
								[&](SDL_Event &e){ return btNoiseClicked(e, 2); }));

	gui->addChild(bgUi);
}

bool NoiseWindow::btMapClicked(SDL_Event &e, int i)
{
	if(e.type == SDL_MOUSEBUTTONUP)
	{
		mapFrame->selectMap(i);

		return true;
	}

	else
		return false;
}

bool NoiseWindow::btNoiseClicked(SDL_Event &e, int i)
{
	if(e.type == SDL_MOUSEBUTTONUP)
	{
		mapFrame->selectNoise(i);

		return true;
	}

	else
		return false;
}
