#include "CustomSdlWindows/NoiseWindow.h"

#include "Map/Map.h"

#include "Ui/UiObject.h"
#include "CustomUiObjects/MapFrame.h"
#include "DefaultUiObjects/UiPanel.h"
#include "DefaultUiObjects/UiButton.h"
#include "DefaultUiObjects/UiLabel.h"

#include "Ui/EventAggregator.h"
#include "Ui/UiEvents/UiEventCode.h"

#include "MyUtils.h"

NoiseWindow::NoiseWindow(Map* mapArr[], int num) : 
	SdlWindow(	"WorldGen", 20, 40, 
				SCREEN_WIDTH, SCREEN_HEIGHT, // window size 
				SCREEN_WIDTH, SCREEN_HEIGHT, // window resolution
				SDL_WINDOW_RESIZABLE, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE) // superclass window constructor
{
	EventAggregator::Instance().getEvent<UiEventCode>().subscribe(
															[&](UiEventCode &c){ customUiEventHandler(c); });

	mapArray = mapArr;
	numMaps = num;

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	createGui();
}

NoiseWindow::~NoiseWindow()
{
	EventAggregator::Instance().getEvent<UiEventCode>().unsubscribe(
															[&](UiEventCode &c){ customUiEventHandler(c); });
}

void NoiseWindow::customUiEventHandler(UiEventCode &c)
{
	if(c.code == UIEVT_RUNNOISEUPDATE)
	{
		doRefresh();
	}
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
	mapFrame = new MapFrame(windowUi, 0, 0, windowUi->getWidth() - SIDEBAR_WIDTH, windowUi->getHeight(), mapArray, numMaps);
	mapFrame->setUiObjectLogicalSize(windowUi->getWidth(), windowUi->getHeight());

	UiPanel *sidebar = new UiPanel(windowUi->getWidth() - SIDEBAR_WIDTH, 0, 
									std::make_shared<SdlTexture>(createDrawnTexture(SIDEBAR_WIDTH, windowUi->getHeight(), 0, 126, 126, 255)));

	sidebar->addChild(new UiButton(30, 30, 
									NULL, 
									std::make_shared<SdlTexture>(createDrawnTexture(30, 30, 0, 255, 0, 255)), 
									std::make_shared<SdlTexture>(createDrawnTexture(30, 30, 115, 255, 115, 255)), 
									std::make_shared<SdlTexture>(createDrawnTexture(30, 30, 0, 205, 0, 255)), 
									//std::make_shared<SdlTexture>(MyUtils::loadTexture(getRenderer(), "Resources\\btNormal.png"), 30, 30), 
									//std::make_shared<SdlTexture>(MyUtils::loadTexture(getRenderer(), "Resources\\btHover.png"), 30, 30), 
									//std::make_shared<SdlTexture>(MyUtils::loadTexture(getRenderer(), "Resources\\btPressed.png"), 30, 30), 
									[&](){ mapFrame->selectMap(0); } ));

	sidebar->addChild(new UiButton(80, 30, 
									NULL, 
									std::make_shared<SdlTexture>(createDrawnTexture(30, 30, 0, 255, 0, 255)), 
									std::make_shared<SdlTexture>(createDrawnTexture(30, 30, 115, 255, 115, 255)), 
									std::make_shared<SdlTexture>(createDrawnTexture(30, 30, 0, 205, 0, 255)), 
									[&](){ mapFrame->selectMap(1); } ));

	sidebar->addChild(new UiButton(130, 30, 
									NULL, 
									std::make_shared<SdlTexture>(createDrawnTexture(30, 30, 0, 255, 0, 255)), 
									std::make_shared<SdlTexture>(createDrawnTexture(30, 30, 115, 255, 115, 255)), 
									std::make_shared<SdlTexture>(createDrawnTexture(30, 30, 0, 205, 0, 255)), 
									[&](){ mapFrame->selectMap(2); } ));

	sidebar->addChild(new UiButton(55, 80, 
									NULL, 
									std::make_shared<SdlTexture>(createDrawnTexture(30, 30, 0, 255, 0, 255)), 
									std::make_shared<SdlTexture>(createDrawnTexture(30, 30, 115, 255, 115, 255)), 
									std::make_shared<SdlTexture>(createDrawnTexture(30, 30, 0, 205, 0, 255)), 
									[&](){ mapFrame->selectNoise(0); } ));

	sidebar->addChild(new UiButton(105, 80, 
									NULL, 
									std::make_shared<SdlTexture>(createDrawnTexture(30, 30, 0, 255, 0, 255)), 
									std::make_shared<SdlTexture>(createDrawnTexture(30, 30, 115, 255, 115, 255)), 
									std::make_shared<SdlTexture>(createDrawnTexture(30, 30, 0, 205, 0, 255)), 
									[&](){ mapFrame->selectNoise(1); } ));

	sidebar->addChild(new UiButton(30, windowUi->getHeight() - 60, 
									new UiLabel(0, 0, "RUN", 12, 255, 0, 0), 
									std::make_shared<SdlTexture>(createDrawnTexture(130, 30, 0, 255, 0, 255)), 
									std::make_shared<SdlTexture>(createDrawnTexture(130, 30, 115, 255, 115, 255)), 
									std::make_shared<SdlTexture>(createDrawnTexture(130, 30, 0, 205, 0, 255)), 
									[&](){ mapFrame->runNoise(); } ));

	mapFrame->addChild(sidebar);
	windowUi->addChild(mapFrame);
}
